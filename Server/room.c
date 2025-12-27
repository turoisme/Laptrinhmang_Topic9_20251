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
		// Automatically join the owner to the room
		if (db_room_join(room_id, owner_id) != 0) {
			return DATABASE_ERROR;
		}
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
	//Check if user is already in a room
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
	if(row){
		mysql_free_result(result);
		db_release_connection(conn);
		return WENT_INTO_ANOTHER_ROOM;
	}
	// Get room ID and check if active
	sprintf(query, "SELECT room_id,is_active FROM rooms WHERE room_name='%s'", param[1]);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	row=mysql_fetch_row(result);
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
	mysql_free_result(result);
	db_release_connection(conn);
	// Join room in database
	if (db_room_join(room_id, account) != 0) {
		return DATABASE_ERROR;
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
		return LIST_ROOMS_SUCCESS;
	} else {
		return DATABASE_ERROR;
	}
}
int handle_leave_room(char *message, int sockfd) {
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}

	int param_count=parse_message(message, NULL);
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
	sprintf(query, "DELETE FROM room_members WHERE user_id='%d' AND room_id='%d'", account, room_id);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
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
	// Might need to do something if bid_price >= buy_now_price

	// Update bid in database
	sprintf(query, "UPDATE items SET current_price='%.2f', bidder='%d' WHERE item_id='%d'", bid_price, account, item_id);
	if(mysql_query(conn,query)){
		mysql_free_result(result);
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	mysql_free_result(result);
	db_release_connection(conn);
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
	// Update item as sold in database
	sprintf(query, "UPDATE items SET is_sold=1, buyer='%d' WHERE item_id='%d'", account, item_id);
	if(mysql_query(conn,query)){
		mysql_free_result(result);
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	mysql_free_result(result);
	db_release_connection(conn);
	return BUY_OK;
}