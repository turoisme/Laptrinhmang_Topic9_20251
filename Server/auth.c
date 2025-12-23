#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "protocol.h"
#include "database.h"
#include "ultility.h"

int verified[1024];             //Hard configured max 1024 users logged in
int verify_account[1024];       //Should be server configurable later

void reset_server_auth() {
	int i;
	for(i=0;i<1024;i++){
		verified[i]=-1;
		verify_account[i]=-1;
	}
}

int is_verified_user(int sockfd,int *account) {
	int i=0;
	for(i=0;i<1024;i++){
		if(verified[i]==sockfd){
			*account=verify_account[i];
			return 1;
		}
	}
	return 0;
}

int find_empty_slot() {
	int i;
	for(i=0;i<1024;i++){
		if(verified[i]==-1)return i;
	}
	return -1;
}

int handle_register(char *message, int sockfd) {
	reset_server_auth(); // For testing purpose only, remove this line in production
	char param[10][100];
	int param_count = parse_message(message, param);
	if(param_count!=3)return FORMAT_ERROR;
	MYSQL* conn=db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	sprintf(query, "INSERT INTO users (username, password) VALUES ('%s', '%s')", param[1], param[2]);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		if(mysql_errno(conn)==1062){
			return USERNAME_EXISTS;
		}
		return DATABASE_ERROR;
	}
	db_release_connection(conn);
	return REGISTER_SUCCESS;
}

int handle_login(char *message, int sockfd) {
	char param[10][100];
	int param_count = parse_message(message, param);
	if(param_count!=3)return FORMAT_ERROR;
	int account;
	if(is_verified_user(sockfd,&account)){
		return ALREADY_LOGGED_IN;
	}
	// Here should be the database verification of username and password
	MYSQL* conn=db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	sprintf(query, "SELECT user_id FROM users WHERE username='%s' AND password='%s'", param[1], param[2]);
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
		return USER_NOT_FOUND;
	}
	mysql_free_result(result);
	db_release_connection(conn);

	int slot=find_empty_slot();
	if(slot<0)return SERVER_OVERLOAD;
	verified[slot]=sockfd;
	verify_account[slot]=atoi(param[1]);
	return LOGIN_SUCCESS;
}

int handle_logout(char *message, int sockfd) {
	char param[10][100];
	int param_count = parse_message(message, param);
	if(param_count!=1)return FORMAT_ERROR;
	for(int i=0;i<1024;i++){
		if(verified[i]==sockfd){
			verified[i]=-1;
			verify_account[i]=-1;
			return LOGOUT_SUCCESS;
		}
	}
	return NOT_LOGGED_IN;
}