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
#define ROOM_NOT_FOUND 312
#define ROOM_CLOSED 313
#define ROOM_CREATED 400
#define LEAVE_SUCCESS 550
#define ITEM_CREATED 600
#define ITEM_DELETED 620
#define ITEM_ALREADY_SOLD 621
#define ACCESS_DENIED 622
#define WENT_INTO_ANOTHER_ROOM 721
#define BID_OK 800
#define BID_TOO_LOW 811
#define BUY_OK 900
#define ITEM_ALREADY_SOLD_BUY 912
#define FORMAT_ERROR 921
void handle_client_message(char *message, int sockfd);

#endif
