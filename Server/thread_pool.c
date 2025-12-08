// thread_pool.c - Thread pool implementation

#include "thread_pool.h"
#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Client handler function - runs in separate thread
void *echo(void *arg) {
    int sockfd;
    int sent_bytes, received_bytes;
    char buff[BUFF_SIZE + 1];
    
    // Get the passed value
    sockfd = *((int *)arg);
    free(arg);
    
    // Detach the thread
    pthread_detach(pthread_self());
    
    // Blocking I/O loop
    received_bytes = recv(sockfd, buff, BUFF_SIZE, 0);
    if (received_bytes < 0) {
        perror("recv() error: ");
    } else if (received_bytes == 0) {
        printf("Connection closed.\n");
    } else {
        buff[received_bytes] = '\0';
        printf("Received: %s\n", buff);
        
        // Process message and send response
        handle_client_message(buff, sockfd);
        
        // Echo to the client
        sent_bytes = send(sockfd, buff, received_bytes, 0);
        if (sent_bytes < 0)
            perror("send() error: ");
    }
    
    close(sockfd);
    return NULL;
}
