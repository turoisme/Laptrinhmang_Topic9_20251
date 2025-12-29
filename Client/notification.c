#include "notification.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static pthread_t notification_thread;
static int thread_running = 0;
static ClientConnection *global_conn = NULL;


void *notification_receiver(void *arg) {
    ClientConnection *conn = (ClientConnection *)arg;
    char *notification = NULL;
    while (thread_running && is_connected(conn)) {
        int result = receive_response(conn, &notification);
        
        if (result <= 0) {
            break;
        }
        
        int code;
        char message[BUFF_SIZE];
        
        if (sscanf(notification, "%d %[^\n]", &code, message) >= 1) {
            if (code >= 1000 && code < 2000) {
                printf("\n[NOTIFICATION] ");
                
                switch (code) {
                    case NOTIFY_BID:
                        printf("New BID: %s\n", message);
                        break;
                    case NOTIFY_BUY:
                        printf("SOLD: %s\n", message);
                        break;
                    case NOTIFY_ITEM_CREATED:
                        printf("New ITEM: %s\n", message);
                        break;
                    case NOTIFY_ITEM_DELETED:
                        printf("ITEM DELETED: %s\n", message);
                        break;
                    case NOTIFY_COUNTDOWN:
                        printf("COUNTDOWN: %s\n", message);
                        break;
                    case NOTIFY_AUCTION_END:
                        printf("AUCTION ENDED: %s\n", message);
                        break;
                    case NOTIFY_USER_JOIN:
                        printf("User JOINED: %s\n", message);
                        break;
                    case NOTIFY_USER_LEAVE:
                        printf("User LEFT: %s\n", message);
                        break;
                    default:
                        printf("%s\n", message);
                        break;
                }
                printf("Enter choice: "); // Re-print prompt
                fflush(stdout);
            }
        }
        
        free(notification);
        notification = NULL;
    }
    
    printf("\n[Notification thread stopped]\n");
    thread_running = 0;
    return NULL;
}

void start_notification_thread(ClientConnection *conn) {
    if (thread_running) {
        printf("Notification thread already running\n");
        return;
    }
    
    global_conn = conn;
    thread_running = 1;
    
    if (pthread_create(&notification_thread, NULL, notification_receiver, conn) != 0) {
        perror("Failed to create notification thread");
        thread_running = 0;
        return;
    }
    
    pthread_detach(notification_thread);
}

void stop_notification_thread() {
    if (thread_running) {
        thread_running = 0;
    }
}
int is_notification_running() {
    return thread_running;
}
