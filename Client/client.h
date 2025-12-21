#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "commands.h"
#include "response_handler.h"

//function
void print_home_menu();
void print_room_menu();
void print_item_menu();
void clear_input_buffer();

#endif
