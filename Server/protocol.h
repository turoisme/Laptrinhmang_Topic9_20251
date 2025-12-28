// protocol.h - Message protocol handler
// Parse and process commands from client

#ifndef PROTOCOL_H
#define PROTOCOL_H

// TODO: Define status codes
// Response codes
#define DATABASE_ERROR 000
#define FUNCTION_IN_DEV 001
#define SERVER_OVERLOAD 002
#define REGISTER_SUCCESS 100
#define USERNAME_EXISTS 111
#define INVALID_INPUT_PARAMETER 112
#define LOGIN_SUCCESS 200
#define USER_NOT_FOUND 211
#define ALREADY_LOGGED_IN 213
#define NOT_LOGGED_IN 214
#define LOGOUT_SUCCESS 220
#define JOIN_OK 300
#define NOT_IN_ROOM 311
#define ROOM_NOT_FOUND 312
#define ROOM_CLOSED 313
#define ROOM_CREATED 400
#define ROOM_EXISTS 411
#define LIST_ROOMS_SUCCESS 500
#define LEAVE_SUCCESS 550
#define ITEM_CREATED 600
#define ITEM_NOT_FOUND 611
#define ITEM_DELETED 620
#define ITEM_ALREADY_SOLD 621
#define ACCESS_DENIED 622
#define LIST_ITEMS_SUCCESS 700
#define WENT_INTO_ANOTHER_ROOM 721
#define BID_OK 800
#define BID_TOO_LOW 811
#define BUY_OK 900
#define ITEM_ALREADY_SOLD_BUY 912
#define FORMAT_ERROR 921

// Notification codes (1xxx range)
#define NOTIFY_BID 1000
#define NOTIFY_BUY 1001
#define NOTIFY_ITEM_CREATED 1002
#define NOTIFY_ITEM_DELETED 1003
#define NOTIFY_COUNTDOWN 1004
#define NOTIFY_AUCTION_END 1005
#define NOTIFY_USER_JOIN 1006
#define NOTIFY_USER_LEAVE 1007

// Function declarations
void handle_client_message(char *message, int sockfd);
int handle_create_item(char *message, int sockfd);
int handle_delete_item(char *message, int sockfd);
int handle_list_items(int sockfd);
void broadcast_to_room(int room_id, int sender_sockfd, int code, const char *message);

#endif
