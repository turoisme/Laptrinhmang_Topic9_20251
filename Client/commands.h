#ifndef COMMANDS_H
#define COMMANDS_H

#include "connection.h"

int cmd_register(ClientConnection *conn, const char *username, const char *password);
int cmd_login(ClientConnection *conn, const char *username, const char *password);
int cmd_create_room(ClientConnection *conn, const char *room_name);
int cmd_join_room(ClientConnection *conn, const char *room_name);
int cmd_leave_room(ClientConnection *conn);
int cmd_list_rooms(ClientConnection *conn);
int cmd_create_item(ClientConnection *conn, const char *item_name, int start_price, int buy_now_price);
int cmd_delete_item(ClientConnection *conn, const char *item_name);
int cmd_list_items(ClientConnection *conn);
int cmd_bid(ClientConnection *conn, const char *item_name, int amount);
int cmd_buy(ClientConnection *conn, const char *item_name);

#endif
