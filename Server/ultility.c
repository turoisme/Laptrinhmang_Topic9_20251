#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ultility.h"
#include "database.h"

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

int search_user_name_by_id(int user_id, char *username) {
	MYSQL* conn=db_get_connection();
	if(!conn)return 0;
	char query[512];
	sprintf(query, "SELECT username FROM users WHERE user_id='%d'", user_id);
	if(mysql_query(conn,query)){
		db_release_connection(conn);
		return 0;
	}
	MYSQL_RES* result=mysql_store_result(conn);
	if(!result){
		db_release_connection(conn);
		return 0;
	}
	MYSQL_ROW row=mysql_fetch_row(result);
	if(!row){
		mysql_free_result(result);
		db_release_connection(conn);
		return 0;
	}
	strcpy(username, row[0]);
	mysql_free_result(result);
	db_release_connection(conn);
	return 1;
}