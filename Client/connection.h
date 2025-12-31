#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 4096
#define DELIMITER "\r\n"

// Connection buffer for stream processing
typedef struct {
    char buffer[BUFF_SIZE * 2];
    int buffer_len;
} ConnectionBuffer;

// Connection state
typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    ConnectionBuffer conn_buf;
    int is_connected;
} ClientConnection;

// Connection functions
int connect_to_server(ClientConnection *conn, const char *ip, int port);
void disconnect_from_server(ClientConnection *conn);
int send_command(ClientConnection *conn, const char *command);
int receive_response(ClientConnection *conn, char **response);
int is_connected(ClientConnection *conn);

#endif
