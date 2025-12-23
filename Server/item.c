// item.c - Item and auction management implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "item.h"
#include "protocol.h"
#include "database.h"
#include "socket_handler.h"

int handle_create_item(char *message, int sockfd) {
	char item_name[100];
	int start_price, buy_now_price;
	
	// Parse: CREATE_ITEM item_name start_price buy_now_price
	if (sscanf(message, "CREATE_ITEM %99s %d %d", item_name, &start_price, &buy_now_price) != 3) {
		return FORMAT_ERROR;
	}
	
	// Validate prices
	if (start_price <= 0 || buy_now_price <= 0 || buy_now_price <= start_price) {
		return INVALID_INPUT_PARAMETER;
	}
	
	// TODO: Get room_id and owner_id from session (sockfd mapping)
	// For now, use dummy values
	int room_id = 1;
	int owner_id = 1;
	
	// Create item in database
	int item_id = db_item_create(item_name, room_id, owner_id, 
	                             (double)start_price, (double)buy_now_price);
	
	if (item_id > 0) {
		// Item created successfully
		return ITEM_CREATED;
	} else {
		// Database error
		return DATABASE_ERROR;
	}
}

int handle_list_items(int sockfd) {
	// TODO: Get room_id from session (sockfd mapping)
	// For now, use dummy room_id = 1
	int room_id = 1;
	
	char *item_list = db_item_list_by_room(room_id);
	
	if (item_list) {
		// Format response with code 700
		char response[4096];
		snprintf(response, sizeof(response), "700 %s", item_list);
		send_message(sockfd, response);
		free(item_list);
		return LIST_ITEMS_SUCCESS;
	} else {
		return DATABASE_ERROR;
	}
}
int handle_delete_item(char *message, int sockfd) {

}

