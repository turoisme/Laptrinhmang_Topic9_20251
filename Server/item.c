#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "item.h"
#include "protocol.h"
#include "database.h"
#include "socket_handler.h"
#include "ultility.h"
#include "auth.h"

int handle_create_item(char *message, int sockfd) {
	char item_name[100];
	int start_price, buy_now_price;
	if (sscanf(message, "CREATE_ITEM %99s %d %d", item_name, &start_price, &buy_now_price) != 3) {
		return FORMAT_ERROR;
	}
	if (start_price <= 0 || buy_now_price <= 0 || buy_now_price <= start_price) {
		return INVALID_INPUT_PARAMETER;
	}
	int owner_id;
	if(!is_verified_user(sockfd,&owner_id)){
		return NOT_LOGGED_IN;
	}
	int room_id = db_user_get_current_room(owner_id);
	if (room_id <= 0) {
		return NOT_IN_ROOM;
	}
	
	int item_id = db_item_create(item_name, room_id, owner_id, 
	                             (double)start_price, (double)buy_now_price);
	
	if (item_id > 0) {
		// Broadcast item creation notification
		MYSQL* conn = db_get_connection();
		if (conn) {
			char username[100] = "";
			char query[512];
			sprintf(query, "SELECT username FROM users WHERE user_id=%d", owner_id);
			if (mysql_query(conn, query) == 0) {
				MYSQL_RES* result = mysql_store_result(conn);
				if (result) {
					MYSQL_ROW row = mysql_fetch_row(result);
					if (row) {
						strncpy(username, row[0], sizeof(username)-1);
					}
					mysql_free_result(result);
				}
			}
			db_release_connection(conn);
			
			if (username[0]) {
				char notify_msg[256];
				snprintf(notify_msg, sizeof(notify_msg), 
				        "%s created item: %s (Start: $%d, BuyNow: $%d)", 
				        username, item_name, start_price, buy_now_price);
				broadcast_to_room(room_id, sockfd, NOTIFY_ITEM_CREATED, notify_msg);
			}
		}
		return ITEM_CREATED;
	} else {
		return DATABASE_ERROR;
	}
}

int handle_list_items(int sockfd) {
	int user_id;
	if(!is_verified_user(sockfd, &user_id)){
		return NOT_LOGGED_IN;
	}
	int room_id = db_user_get_current_room(user_id);
	if (room_id <= 0) {
		return NOT_IN_ROOM;
	}

	char *item_list = db_item_list_by_room(room_id);

	if (item_list) {
		char response[4096];
		snprintf(response, sizeof(response), "700 %s", item_list);
		send_message(sockfd, response);
		free(item_list);
		return 0;
	} else {
		return DATABASE_ERROR;
	}
}
int handle_delete_item(char *message, int sockfd) {
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=2){
		return FORMAT_ERROR;
	}
	MYSQL* conn = db_get_connection();
	if(!conn){
		return DATABASE_ERROR;
	}
	char query[512];
	sprintf(query, "SELECT item_id,is_sold,owner_id FROM items WHERE item_name='%s'",param[1]);
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
	int owner_id = atoi(row[2]);
	if(owner_id!=account){
		mysql_free_result(result);
		db_release_connection(conn);
		return ACCESS_DENIED;
	}
	if(is_sold){
		mysql_free_result(result);
		db_release_connection(conn);
		return ITEM_ALREADY_SOLD;
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
	
	// Delete item from database
	sprintf(query, "DELETE FROM items WHERE item_id='%d'", item_id);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return DATABASE_ERROR;
	}
	db_release_connection(conn);
	
	// Broadcast item deletion
	if (room_id > 0 && username[0]) {
		char notify_msg[256];
		snprintf(notify_msg, sizeof(notify_msg), 
		        "%s deleted item: %s", username, param[1]);
		broadcast_to_room(room_id, sockfd, NOTIFY_ITEM_DELETED, notify_msg);
	}
	
	return ITEM_DELETED;
}

