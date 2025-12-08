// protocol.c - Protocol handler implementation

#include "protocol.h"
#include "socket_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// TODO: Implement protocol handler functions

// Main handler for incoming client messages
void handle_client_message(char *message, int sockfd) {
    // Simple echo for now - will be replaced with actual command parsing
    char response[BUFF_SIZE];
    snprintf(response, BUFF_SIZE, "Server received: %s", message);
    send_message(sockfd, response);
}
