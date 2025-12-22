// protocol.c - Protocol handler implementation

#include "protocol.h"
#include "socket_handler.h"
#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
#include "room.h"
// send formatted response
void send_response(int sockfd, int code, const char *message) {
    char response[BUFF_SIZE];
    snprintf(response, BUFF_SIZE, "%d %s", code, message);
    send_message(sockfd, response);
}

// Handle REGISTER 
void handle_register(char *message, int sockfd) {
    char *code = takeAuthCommand(sockfd, message);
    int result = atoi(code);    
    if (result == 100) {
        send_response(sockfd, REGISTER_SUCCESS, "Registration successful");
    } else if (result == 111) {
        send_response(sockfd, USERNAME_EXISTS, "Username already exists");
    } else if (result == 112) {
        send_response(sockfd, INVALID_INPUT_PARAMETER, "Invalid input parameters");
    } else if (result == 213) {
        send_response(sockfd, ALREADY_LOGGED_IN, "Already logged in");
    } else {
        send_response(sockfd, DATABASE_ERROR, "Database error");
    }
}

// Handle LOGIN 
void handle_login(char *message, int sockfd) {
    char *code = takeAuthCommand(sockfd, message);
    int result = atoi(code);
    
    if (result == 200) {
        send_response(sockfd, LOGIN_SUCCESS, "Login successful");
    } else if (result == 211) {
        send_response(sockfd, USER_NOT_FOUND, "Invalid username or password");
    } else if (result == 213) {
        send_response(sockfd, ALREADY_LOGGED_IN, "Already logged in");
    } else if (result == 112) {
        send_response(sockfd, INVALID_INPUT_PARAMETER, "Invalid input parameters");
    } else {
        send_response(sockfd, DATABASE_ERROR, "Database error");
    }
}

// Handle CREATE_ROOM 
void handle_create_room(char *message, int sockfd) {
    char *code = takeRoomCommand(sockfd, message);
    int result = atoi(code);
    
    if (result == 400) {
        send_response(sockfd, ROOM_CREATED, "Room created successfully");
    } else if (result == 721) {
        send_response(sockfd, WENT_INTO_ANOTHER_ROOM, "Already in another room");
    } else {
        send_response(sockfd, DATABASE_ERROR, "Failed to create room");
    }
}

// Handle JOIN_ROOM
void handle_join_room(char *message, int sockfd) {
    char *code = takeRoomCommand(sockfd, message);
    int result = atoi(code);
    
    if (result == 300) {
        send_response(sockfd, JOIN_OK, "Joined room successfully");
    } else if (result == 313) {
        send_response(sockfd, ROOM_CLOSED, "Room not found");
    } else if (result == 721) {
        send_response(sockfd, WENT_INTO_ANOTHER_ROOM, "Already in another room");
    } else {
        send_response(sockfd, DATABASE_ERROR, "Failed to join room");
    }
}

// Handle CREATE_ITEM 
void handle_create_item(char *message, int sockfd) {
    //Tan
}

// Handle DELETE_ITEM 
void handle_delete_item(char *message, int sockfd) {
   //Vinh
}

// Handle BID 
void handle_bid(char *message, int sockfd) {
  //Vinh
}

// Handle BUY 
void handle_buy(char *message, int sockfd) {
    //Vinh 
}

// Handle LIST_ROOMS
void handle_list_rooms(int sockfd) {
    //Tan 
}

// Handle LIST_ITEMS 
void handle_list_items(int sockfd) {
    //Tan
}

// Handle client message
void handle_client_message(char *message, int sockfd) {
    char command[50];
    
    // Extract first word as command
    if (sscanf(message, "%49s", command) != 1) {
        send_response(sockfd, FORMAT_ERROR, "Empty command");
        return;
    }
    
    // Route to appropriate handler
    if (strcmp(command, "REGISTER") == 0) {
        handle_register(message, sockfd);
    } else if (strcmp(command, "LOGIN") == 0) {
        handle_login(message, sockfd);
    } else if (strcmp(command, "CREATE_ROOM") == 0) {
        handle_create_room(message, sockfd);
    } else if (strcmp(command, "JOIN_ROOM") == 0) {
        handle_join_room(message, sockfd);
    } else if (strcmp(command, "LIST_ROOMS") == 0) {
        handle_list_rooms(sockfd);
    } else if (strcmp(command, "CREATE_ITEM") == 0) {
        handle_create_item(message, sockfd);
    } else if (strcmp(command, "DELETE_ITEM") == 0) {
        handle_delete_item(message, sockfd);
    } else if (strcmp(command, "LIST_ITEMS") == 0) {
        handle_list_items(sockfd);
    } else if (strcmp(command, "BID") == 0) {
        handle_bid(message, sockfd);
    } else if (strcmp(command, "BUY") == 0) {
        handle_buy(message, sockfd);
    } else {
        send_response(sockfd, FORMAT_ERROR, "Unknown command");
    }
}
