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
	char *param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=2){
		return FORMAT_ERROR;
	}
	int account;
	if(!is_verified_user(sockfd, &account)){
		return NOT_LOGGED_IN;
	}
	
	char query[512];
	sprintf(query, "SELECT room_id FROM rooms WHERE room_name='%s' AND is_active=1", param[1]);
	MYSQL* conn = db_get_connection();
	if(!conn)return DATABASE_ERROR;
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

	int room_id = atoi(row[0]);
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
	
	int room_id = db_user_get_current_room(account);
	if(room_id <= 0){
		return NOT_IN_ROOM;
	}
	
	// Leave room in database
	if (db_room_leave(room_id, account) != 0) {
		return DATABASE_ERROR;
	}
	return LEAVE_SUCCESS;
}

int handle_bid(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_buy(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}