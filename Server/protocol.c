// protocol.c - Protocol handler implementation

#include "protocol.h"
#include "socket_handler.h"
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "room.h"
// Helper function to send formatted response
char *parse_response(int code){
    switch(code){
        case 000:return "Empty command";
        case 001:return "Function in development";
        case 002:return "Server overload";
        case 100:return "Register successful";
        case 111:return "Username already exists";
        case 112:return "Invalid input parameter";
        case 200:return "Login successful";
        case 211:return "User not found";
        case 213:return "Already logged in";
        case 214:return "Have not logged in";
        case 220:return "Logout successful";
        case 300:return "Join room successful";
        case 312:return "Room not found";
        case 313:return "Room closed";
        case 400:return "Room created successfully";
        case 411:return "Room already exists";
        case 500:return "Room list";
        case 600:return "Item created successfully";
        case 620:return "Item deleted successfully";
        case 621:return "Item already sold";
        case 622:return "Access denied";
        case 700:return "Item list";
        case 721:return "Went into another room";
        case 800:return "Bid successful";
        case 811:return "Bid too low";
        case 900:return "Buy successful";
        case 912:return "Item already sold";
        case 921:return "Format error";
        default:return "Unknown status code";
    }
}
void send_response(int sockfd, int code) {
    char response[BUFF_SIZE];
    snprintf(response, BUFF_SIZE, "%d %s", code, parse_response(code));
    send_message(sockfd, response);
}

void handle_client_message(char *message, int sockfd) {
    char command[50];
    
    // Extract first word as command
    if (sscanf(message, "%49s", command) != 1) {
        send_response(sockfd, FORMAT_ERROR);
        return;
    }
    
    // Route to appropriate handler
    if (strcmp(command, "REGISTER") == 0) {
        send_response(sockfd, handle_register(message, sockfd));
    } else if (strcmp(command, "LOGIN") == 0) {
        send_response(sockfd, handle_login(message, sockfd));
    } else if (strcmp(command, "LOGOUT") == 0) {
        send_response(sockfd, handle_logout(message, sockfd));
    } else if (strcmp(command, "CREATE_ROOM") == 0) {
        send_response(sockfd, handle_create_room(message, sockfd));
    } else if (strcmp(command, "JOIN_ROOM") == 0) {
        send_response(sockfd, handle_join_room(message, sockfd));
    } else if (strcmp(command, "LIST_ROOMS") == 0) {
        send_response(sockfd, handle_list_rooms(sockfd));
    } else if (strcmp(command, "LEAVE_ROOM") == 0) {
        send_response(sockfd, handle_leave_room(message, sockfd));
    } else if (strcmp(command, "CREATE_ITEM") == 0) {
        send_response(sockfd, handle_create_item(message, sockfd));
    } else if (strcmp(command, "DELETE_ITEM") == 0) {
        send_response(sockfd, handle_delete_item(message, sockfd));
    } else if (strcmp(command, "LIST_ITEMS") == 0) {
        send_response(sockfd, handle_list_items(sockfd));
    } else if (strcmp(command, "BID") == 0) {
        send_response(sockfd, handle_bid(message, sockfd));
    } else if (strcmp(command, "BUY") == 0) {
        send_response(sockfd, handle_buy(message, sockfd));
    } else {
        send_response(sockfd, FORMAT_ERROR);
    }
}
