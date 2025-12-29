#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
#include "protocol.h"
#include "database.h"
#include "socket_handler.h"
#include "ultility.h"
#include "auth.h"
int handle_create_room(char *message, int sockfd) {
	char room_name[100];
	if (sscanf(message, "CREATE_ROOM %99s", room_name) != 1) {
		return FORMAT_ERROR;
	}
	int owner_id;
	int room_id;
	if(!is_verified_user(sockfd,&owner_id)){
		return NOT_LOGGED_IN;
	}
	room_id = db_room_create(room_name, owner_id, 0, NULL);
	if (room_id > 0) {
		return ROOM_CREATED;
	} else if (room_id == -2) {
		return ROOM_EXISTS;
	} else {
		return DATABASE_ERROR;
	}
}
int handle_join_room(char *message, int sockfd) {
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=2){
		return FORMAT_ERROR;
	}
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	MYSQL* conn = db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	
	// Get room ID and check if active
	sprintf(query, "SELECT room_id,is_active FROM rooms WHERE room_name='%s'", param[1]);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_RES* result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_ROW row=mysql_fetch_row(result);
	if(!row){
		mysql_free_result(result);
		db_release_connection(conn);
		return ROOM_NOT_FOUND;
	}
	// Get room_id and is_active
	int room_id = atoi(row[0]);
	int is_active = atoi(row[1]);
	if(!is_active){
		mysql_free_result(result);
		db_release_connection(conn);
		return ROOM_CLOSED;
	}
	// Join room in database
	sprintf(query, "INSERT INTO room_members (room_id, user_id) VALUES ('%d', '%d')", room_id, account);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	
	// Get username for notification
	conn = db_get_connection();
	if (conn) {
		sprintf(query, "SELECT username FROM users WHERE user_id=%d", account);
		if (mysql_query(conn, query) == 0) {
			result = mysql_store_result(conn);
			if (result) {
				row = mysql_fetch_row(result);
				if (row) {
					char notify_msg[256];
					snprintf(notify_msg, sizeof(notify_msg), "%s joined %s", row[0], param[1]);
					broadcast_to_room(room_id, sockfd, NOTIFY_USER_JOIN, notify_msg);
				}
				mysql_free_result(result);
			}
		}
		db_release_connection(conn);
	}
	
	return JOIN_OK;
}

int handle_list_rooms(int sockfd) {
	char *room_list = db_room_list_all();
	
	if (room_list) {
		// Format response with code 500
		char response[4096];
		snprintf(response, sizeof(response), "500 %s", room_list);
		send_message(sockfd, response);
		free(room_list);
		// Return 0 to indicate we already sent the response
		return 0;
	} else {
		return DATABASE_ERROR;
	}
}
int handle_leave_room(char *message, int sockfd) {
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=1){
		return FORMAT_ERROR;
	}
	MYSQL* conn = db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	sprintf(query, "SELECT room_id FROM room_members WHERE user_id='%d'", account);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_RES* result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_ROW row=mysql_fetch_row(result);
	if(!row){
		mysql_free_result(result);
		db_release_connection(conn);
		return NOT_IN_ROOM;
	}
	int room_id = atoi(row[0]);
	mysql_free_result(result);
	// Get username for notification
	char username[100] = "";
	sprintf(query, "SELECT username FROM users WHERE user_id='%d'", account);
	if (mysql_query(conn, query) == 0) {
		result = mysql_store_result(conn);
		if (result) {
			row = mysql_fetch_row(result);
			if (row) {
				strncpy(username, row[0], sizeof(username)-1);
			}
			mysql_free_result(result);
		}
	}
	
	// Get room name for notification
	char room_name[100] = "";
	sprintf(query, "SELECT room_name FROM rooms WHERE room_id=%d", room_id);
	if (mysql_query(conn, query) == 0) {
		result = mysql_store_result(conn);
		if (result) {
			row = mysql_fetch_row(result);
			if (row) {
				strncpy(room_name, row[0], sizeof(room_name)-1);
			}
			mysql_free_result(result);
		}
	}
	
	// Delete from room_members FIRST (before broadcast)
	sprintf(query, "DELETE FROM room_members WHERE user_id='%d' AND room_id='%d'", account, room_id);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	
	// Broadcast AFTER leaving (to remaining members only)
	if (username[0] && room_name[0]) {
		char notify_msg[256];
		snprintf(notify_msg, sizeof(notify_msg), "%s left %s", username, room_name);
		broadcast_to_room(room_id, sockfd, NOTIFY_USER_LEAVE, notify_msg);
	}
	
	db_release_connection(conn);
	
	return LEAVE_SUCCESS;
}

int handle_bid(char *message, int sockfd) {
	// Search for verified user
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	// Parse parameters
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=3){
		return FORMAT_ERROR;
	}
	MYSQL* conn = db_get_connection();
	if(!conn){
		return DATABASE_ERROR;
	}
	// Check item existence and status
	char query[512];
	sprintf(query, "SELECT item_id,current_price,is_sold FROM items WHERE item_name='%s'",param[1]);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_RES* result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_ROW row=mysql_fetch_row(result);
	if(!row){
		mysql_free_result(result);
		db_release_connection(conn);
		return ITEM_NOT_FOUND;
	}
	// Get item details, check status
	int item_id = atoi(row[0]);
	double current_price = atof(row[1]);
	double bid_price = atof(param[2]);
	int is_sold = atoi(row[2]);
	if(is_sold){
		mysql_free_result(result);
		db_release_connection(conn);
		return ITEM_ALREADY_SOLD;
	}
	// Check bid price
	if(bid_price <= current_price){
		mysql_free_result(result);
		db_release_connection(conn);
		return BID_TOO_LOW;
	}
	// update bid and extend expires_at by 3 minutes
	sprintf(query, "UPDATE items SET current_price='%.2f', current_bidder_id='%d', expires_at=DATE_ADD(NOW(), INTERVAL 3 MINUTE) WHERE item_id='%d'", bid_price, account, item_id);
	if(mysql_query(conn,query)){
		mysql_free_result(result);
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	mysql_free_result(result);
	
	// Get room_id and username for notification
	int room_id = db_user_get_current_room(account);
	char username[100] = "";
	sprintf(query, "SELECT username FROM users WHERE user_id=%d", account);
	if (mysql_query(conn, query) == 0) {
		result = mysql_store_result(conn);
		if (result) {
			row = mysql_fetch_row(result);
			if (row) {
				strncpy(username, row[0], sizeof(username)-1);
			}
			mysql_free_result(result);
		}
	}
	db_release_connection(conn);
	
	// Broadcast bid notification
	if (room_id > 0 && username[0]) {
		char notify_msg[256];
		snprintf(notify_msg, sizeof(notify_msg), 
		        "%s bid $%.2f on %s", username, bid_price, param[1]);
		broadcast_to_room(room_id, sockfd, NOTIFY_BID, notify_msg);
	}
	
	return BID_OK;
}

int handle_buy(char *message, int sockfd) {
	// Search for verified user
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	// Parse parameters
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=2){
		return FORMAT_ERROR;
	}
	MYSQL* conn = db_get_connection();
	if(!conn){
		return DATABASE_ERROR;
	}
	// Check item existence and status
	char query[512];
	sprintf(query, "SELECT item_id,is_sold FROM items WHERE item_name='%s'",param[1]);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_RES* result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	MYSQL_ROW row=mysql_fetch_row(result);
	if(!row){
		mysql_free_result(result);
		db_release_connection(conn);
		return ITEM_NOT_FOUND;
	}
	// Get item details, check status
	int item_id = atoi(row[0]);
	int is_sold = atoi(row[1]);
	if(is_sold){
		mysql_free_result(result);
		db_release_connection(conn);
		return ITEM_ALREADY_SOLD_BUY;
	}
	// Update item as sold in database (no buyer column, just mark sold)
	sprintf(query, "UPDATE items SET is_sold=1, current_bidder_id='%d' WHERE item_id='%d'", account, item_id);
	if(mysql_query(conn,query)){
		mysql_free_result(result);
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	mysql_free_result(result);
	
	// Get room_id and username for notification
	int room_id = db_user_get_current_room(account);
	char username[100] = "";
	sprintf(query, "SELECT username FROM users WHERE user_id=%d", account);
	if (mysql_query(conn, query) == 0) {
		result = mysql_store_result(conn);
		if (result) {
			row = mysql_fetch_row(result);
			if (row) {
				strncpy(username, row[0], sizeof(username)-1);
			}
			mysql_free_result(result);
		}
	}
	db_release_connection(conn);
	
	// Broadcast buy notification
	if (room_id > 0 && username[0]) {
		char notify_msg[256];
		snprintf(notify_msg, sizeof(notify_msg), 
		        "%s bought item: %s", username, param[1]);
		broadcast_to_room(room_id, sockfd, NOTIFY_BUY, notify_msg);
	}
	
	return BUY_OK;
}