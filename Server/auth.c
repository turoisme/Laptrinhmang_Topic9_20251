#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "protocol.h"

int verified[1024];             //Hard configured max 1024 users logged in
char verify_account[1024][100]; //Should be server configurable later

// This can be an ultility function to parse messages for all handlers
// Consider make a new ultility.c file later
int parse_message(char *message, char params[10][100]) {
	int paramCount = 0;
	char *token = strtok(message, " ");
	while (token != NULL && paramCount < 10) {
		strcpy(params[paramCount], token);
		paramCount++;
		token = strtok(NULL, " ");
	}
	return paramCount;
}

void reset_server_auth() {
	for(int i=0;i<1024;i++){
		verified[i]=-1;
		strcpy(verify_account[i],"");
	}
}

int is_verified_user(int sockfd,char *account) {
	int i=0;
	for(int i=0;i<1024;i++){
		if(verified[i]==sockfd){
			strcpy(account,verify_account[i]);
			return 1;
		}
	}
	return 0;
}

int find_empty_slot() {
	for(int i=0;i<1024;i++){
		if(verified[i]==-1)return i;
	}
	return -1;
}

int handle_register(char *message, int sockfd) {
	return REGISTER_SUCCESS; //Not implemented yet
	return FUNCTION_IN_DEV;  // Put here to remind that this function is not fully implemented
	char param[10][100];
	int paramCount = parse_message(message, param);
	if(paramCount!=3)return FORMAT_ERROR;
	MYSQL *conn=db_get_connection();
	if(!conn)return DATABASE_ERROR;
	char query[512];
	sprintf(query, "SELECT * FROM users WHERE username='%s' AND password='%s'", param[1], param[2]);
	
	db_release_connection(conn);
	
}

int handle_login(char *message, int sockfd) {
	char param[10][100];
	int paramCount = parse_message(message, param);
	if(paramCount!=3)return FORMAT_ERROR;
	char account[100];
	if(is_verified_user(sockfd,account)){
		return ALREADY_LOGGED_IN;
	}
	// Here should be the database verification of username and password
	// Now we just simulate successful login
	int slot;
	if(slot=find_empty_slot()<0)return SERVER_OVERLOAD;
	verified[slot]=sockfd;
	strcpy(verify_account[slot],param[1]);
	return LOGIN_SUCCESS;
	return FUNCTION_IN_DEV; // Put here to remind that this function is not fully implemented
}

int handle_logout(char *message, int sockfd) {
	char param[10][100];
	int paramCount = parse_message(message, param);
	if(paramCount!=1)return FORMAT_ERROR;
	for(int i=0;i<1024;i++){
		if(verified[i]==sockfd){
			verified[i]=-1;
			strcpy(verify_account[i],"");
			return LOGOUT_SUCCESS;
		}
	}
	return LOGOUT_FAILURE;
}