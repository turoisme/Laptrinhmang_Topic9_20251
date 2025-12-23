#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
#include "protocol.h"
#include "database.h"
#include "ultility.h"

int handle_create_room(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_join_room(char *message, int sockfd) {
	char *param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=2){
		return FORMAT_ERROR;
	}
	int account;
	
	char query[512];
	sprintf(query, "SELECT room_id FROM rooms WHERE room_id='%s'", param[1]);
	MYSQL* conn=db_get_connection();
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

	mysql_free_result(result);
	db_release_connection(conn);
	return JOIN_OK;
}

int handle_list_rooms(int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_leave_room(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_bid(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_buy(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}