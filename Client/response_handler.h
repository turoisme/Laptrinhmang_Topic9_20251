#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <stdio.h>
#include "connection.h"

// Response codes (matching server protocol.h)
#define DATABASE_ERROR 0
#define REGISTER_SUCCESS 100
#define USERNAME_EXISTS 111
#define INVALID_INPUT_PARAMETER 112
#define LOGIN_SUCCESS 200
#define USER_NOT_FOUND 211
#define ALREADY_LOGGED_IN 213
#define JOIN_OK 300
#define ROOM_CLOSED 313
#define ROOM_CREATED 400
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

// Parse and display server response
void parse_and_display_response(const char *response);

#endif
