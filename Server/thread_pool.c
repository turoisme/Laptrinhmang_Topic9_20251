#include "thread_pool.h"
#include "protocol.h"
#include "socket_handler.h"
#include "auth.h"
#include "database.h"
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
    
    // Cleanup - clear user from rooms if they were logged in
    int user_id = -1;
    for(int i=0;i<1024;i++){
        if(verified[i]==sockfd){
            user_id = verify_account[i];
            verified[i]=-1;
            verify_account[i]=-1;
            break;
        }
    }
    if(user_id > 0) {
        db_user_leave_all_rooms(user_id);
        printf("Thread %lu: Cleared user %d from all rooms on disconnect\n", pthread_self(), user_id);
    }
    
    if (message) free(message);
    close(sockfd);
    printf("Thread %lu: Closed socket %d and exiting\n", pthread_self(), sockfd);
    return NULL;
}
