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
	if(!is_verified_user(sockfd,&account)){
		return NOT_LOGGED_IN;
	}
	MYSQL* conn=db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	sprintf(query, "INSERT INTO room_members (room_id, user_id) VALUES ('%s', %d)", param[1], account);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		if(mysql_errno(conn)==1452){
			return ROOM_NOT_FOUND;
		}
		return DATABASE_ERROR;
	}
	db_release_connection(conn);
	return JOIN_OK;
}

int handle_list_rooms(int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_leave_room(char *message, int sockfd) {
	char param[10][100];
	int param_count=parse_message(message, param);
	if(param_count!=1){
		return FORMAT_ERROR;
	}
	MYSQL* conn=db_get_connection();
	if(!conn)return DATABASE_ERROR;
	int account;
	if(!is_verified_user(sockfd,&account)){
		db_release_connection(conn);
		return NOT_LOGGED_IN;
	}
	char query[512];
	sprintf(query, "DELETE FROM room_members WHERE room_id='%s' AND user_id=%d", param[1], account);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		if(mysql_errno(conn)==1452){
			return ROOM_NOT_FOUND;
		}
		return DATABASE_ERROR;
	}
	db_release_connection(conn);
	return LEAVE_SUCCESS;
}

int handle_bid(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}

int handle_buy(char *message, int sockfd) {
	return FUNCTION_IN_DEV;
}