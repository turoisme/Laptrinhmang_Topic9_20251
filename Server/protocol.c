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
void send_response(int sockfd, int code, const char *message) {
    char response[BUFF_SIZE];
    snprintf(response, BUFF_SIZE, "%d %s", code, message);
    send_message(sockfd, response);
}

// Handle REGISTER command
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

// Handle LOGIN command
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

// Handle CREATE_ROOM command
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

// Handle JOIN_ROOM command
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

// Handle CREATE_ITEM command
void handle_create_item(char *message, int sockfd) {
    char item_name[100];
    int start_price, buy_now_price;
    
    // Parse: CREATE_ITEM item_name start_price buy_now_price
    if (sscanf(message, "CREATE_ITEM %99s %d %d", item_name, &start_price, &buy_now_price) != 3) {
        send_response(sockfd, FORMAT_ERROR, "Format: CREATE_ITEM <name> <start_price> <buy_now_price>");
        return;
    }
    
    if (start_price <= 0 || buy_now_price <= 0 || buy_now_price <= start_price) {
        send_response(sockfd, INVALID_INPUT_PARAMETER, "Invalid prices");
        return;
    }
    
    // TODO: Implement actual item creation
    send_response(sockfd, ITEM_CREATED, item_name);
}

// Handle DELETE_ITEM command
void handle_delete_item(char *message, int sockfd) {
    char item_name[100];
    
    // Parse: DELETE_ITEM item_name
    if (sscanf(message, "DELETE_ITEM %99s", item_name) != 1) {
        send_response(sockfd, FORMAT_ERROR, "Format: DELETE_ITEM <item_name>");
        return;
    }
    
    // TODO: Implement actual item deletion
    send_response(sockfd, ITEM_DELETED, item_name);
}

// Handle BID command
void handle_bid(char *message, int sockfd) {
    char item_name[100];
    int bid_amount;
    
    // Parse: BID item_name amount
    if (sscanf(message, "BID %99s %d", item_name, &bid_amount) != 2) {
        send_response(sockfd, FORMAT_ERROR, "Format: BID <item_name> <amount>");
        return;
    }
    
    if (bid_amount <= 0) {
        send_response(sockfd, INVALID_INPUT_PARAMETER, "Bid amount must be positive");
        return;
    }
    
    // TODO: Implement actual bidding logic
    send_response(sockfd, BID_OK, item_name);
}

// Handle BUY command
void handle_buy(char *message, int sockfd) {
    char item_name[100];
    
    // Parse: BUY item_name
    if (sscanf(message, "BUY %99s", item_name) != 1) {
        send_response(sockfd, FORMAT_ERROR, "Format: BUY <item_name>");
        return;
    }
    
    // TODO: Implement actual buy now logic
    send_response(sockfd, BUY_OK, item_name);
}

// Handle LIST_ROOMS command
void handle_list_rooms(int sockfd) {
    // TODO: Implement actual room listing
    send_response(sockfd, 300, "No rooms available");
}

// Handle LIST_ITEMS command
void handle_list_items(int sockfd) {
    // TODO: Implement actual item listing
    send_response(sockfd, 600, "No items available");
}

// Main handler for incoming client messages
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
