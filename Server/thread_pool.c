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
    sockfd = *((int *)arg);
    free(arg);
    pthread_detach(pthread_self());

    ConnectionBuffer conn_buf;
    memset(&conn_buf, 0, sizeof(ConnectionBuffer));
    printf("Thread %lu: Started handling client socket %d\n", pthread_self(), sockfd);
    
    // Blocking I/O loop - handle multiple messages from client
    while (1) {
        int result = recv_message(sockfd, &message, &conn_buf);
        if (result < 0) {
            printf("Thread %lu: Client disconnected (socket %d)\n", pthread_self(), sockfd);
            break;
        } else if (result == 0) {
            continue;
        } else {
            printf("Thread %lu: Received message: %s\n", pthread_self(), message);
            handle_client_message(message, sockfd);
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
