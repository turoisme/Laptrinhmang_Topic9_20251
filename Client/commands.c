// commands.c - Client commands implementation

#include "commands.h"
#include <stdio.h>
#include <string.h>

// Register new user
int cmd_register(ClientConnection *conn, const char *username, const char *password) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "REGISTER %s %s", username, password);
    return send_command(conn, command);
}

// Login user
int cmd_login(ClientConnection *conn, const char *username, const char *password) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "LOGIN %s %s", username, password);
    return send_command(conn, command);
}

// Create auction room
int cmd_create_room(ClientConnection *conn, const char *room_name) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "CREATE_ROOM %s", room_name);
    return send_command(conn, command);
}

// Join auction room
int cmd_join_room(ClientConnection *conn, const char *room_name) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "JOIN_ROOM %s", room_name);
    return send_command(conn, command);
}

// Leave current room
int cmd_leave_room(ClientConnection *conn) {
    return send_command(conn, "LEAVE_ROOM");
}

// List all rooms
int cmd_list_rooms(ClientConnection *conn) {
    return send_command(conn, "LIST_ROOMS");
}

// Create auction item
int cmd_create_item(ClientConnection *conn, const char *item_name, int start_price, int buy_now_price) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "CREATE_ITEM %s %d %d", item_name, start_price, buy_now_price);
    return send_command(conn, command);
}

// Delete item
int cmd_delete_item(ClientConnection *conn, const char *item_name) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "DELETE_ITEM %s", item_name);
    return send_command(conn, command);
}

// List all items in current room
int cmd_list_items(ClientConnection *conn) {
    return send_command(conn, "LIST_ITEMS");
}

// Place bid on item
int cmd_bid(ClientConnection *conn, const char *item_name, int amount) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "BID %s %d", item_name, amount);
    return send_command(conn, command);
}

// Buy item immediately
int cmd_buy(ClientConnection *conn, const char *item_name) {
    char command[BUFF_SIZE];
    snprintf(command, BUFF_SIZE, "BUY %s", item_name);
    return send_command(conn, command);
}
