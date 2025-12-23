#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
#include "protocol.h"
#include "database.h"
#include "socket_handler.h"

int handle_create_room(char *message, int sockfd) {
	char room_name[100];
	if (sscanf(message, "CREATE_ROOM %99s", room_name) != 1) {
		return FORMAT_ERROR;
	}
	// TODO: Get user_id from session (sockfd mapping)
	// For now, use dummy user_id = 1
	int owner_id = 1;
	int room_id = db_room_create(room_name, owner_id, 0, NULL);
	if (room_id > 0) {
		return ROOM_CREATED;
	} else if (room_id == -2) {
		return ROOM_EXISTS;
	} else {
		return DATABASE_ERROR;
	}
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
int handle_join_room(char *message, int sockfd) {
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