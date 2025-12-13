// thread_pool.c - Thread pool implementation

#include "thread_pool.h"
#include "protocol.h"
#include "socket_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Client handler function - runs in separate thread
void *echo(void *arg) {
    int sockfd;
    char *message = NULL;
    
    // Get the passed value
    sockfd = *((int *)arg);
    free(arg);
    
    // Detach the thread
    pthread_detach(pthread_self());
    
    // Initialize connection buffer for this client
    ConnectionBuffer conn_buf;
    memset(&conn_buf, 0, sizeof(ConnectionBuffer));
    
    printf("Thread %lu: Started handling client socket %d\n", pthread_self(), sockfd);
    
    // Blocking I/O loop - handle multiple messages from client
    while (1) {
        // Use recv_message to properly handle streaming protocol
        int result = recv_message(sockfd, &message, &conn_buf);
        
        if (result < 0) {
            // Connection error or closed
            printf("Thread %lu: Client disconnected (socket %d)\n", pthread_self(), sockfd);
            break;
        } else if (result == 0) {
            // Empty message, continue
            continue;
        } else {
            // Valid message received
            printf("Thread %lu: Received message: %s\n", pthread_self(), message);
            
            // Process message and send response
            handle_client_message(message, sockfd);
            
            // Free the message allocated by recv_message
            free(message);
            message = NULL;
        }
    }
    
    // Cleanup
    if (message) free(message);
    close(sockfd);
    printf("Thread %lu: Closed socket %d and exiting\n", pthread_self(), sockfd);
    return NULL;
}
