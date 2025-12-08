#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFF_SIZE 4096
#define DELIMITER "\r\n"
#define MAX_USERS 100

// Buffer to process the remaining data after encountering \r\n
typedef struct {
    char buffer[BUFF_SIZE * 2];
    int buffer_len;
} ConnectionBuffer;

// Thread arguments for each client connection
typedef struct {
    int sockfd;
    ConnectionBuffer conn_buf;
} ThreadArgs;

// Active user structure
typedef struct {
    int sockfd;
    int user_id;
    char username[50];
    int is_logged_in;
    int current_room_id;
} ActiveUser;

// Global active users array
extern ActiveUser active_users[MAX_USERS];

// Function declarations
int create_server_socket(int port);
int accept_client(int listen_sock);
int send_message(int sockfd, const char *message);
int recv_message(int sockfd, char **message, ConnectionBuffer *conn_buf);
void close_socket(int sockfd);

#endif
