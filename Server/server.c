// server.c - Server main file
// Start server, listen for client connections

#include "server.h"
#include "socket_handler.h"
#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int main(int argc, char *argv[]) {
    int listen_sock, *conn_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    pthread_t tid;
    
    // Check command line arguments
    if (argc != 2) {
        printf("Usage: %s <Port_Number>\n", argv[0]);
        printf("Example: %s 5500\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {
        printf("Invalid port number. Must be between 1-65535\n");
        exit(EXIT_FAILURE);
    }
    
    // Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error: ");
        exit(EXIT_FAILURE);
    }
    
    // Step 2: Bind address to socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error: ");
        exit(EXIT_FAILURE);
    }
    
    // Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1) {
        perror("listen() error: ");
        exit(EXIT_FAILURE);
    }
    
    printf("Server started at the port %d\n", port);
    
    // Step 4: Communicate with clients
    while (1) {
        conn_sock = (int *)malloc(sizeof(int));
        if ((*conn_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &sin_size)) == -1)
            perror("accept() error: ");
        else {
            int client_port;
            char client_ip[INET_ADDRSTRLEN];
            
            if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) == NULL) {
                perror("inet_ntop() error:");
            } else {
                client_port = ntohs(client_addr.sin_port);
                printf("You got a connection from %s:%d\n", client_ip, client_port);
            }
            
            // For each client, spawns a thread, and the thread handles the new client
            pthread_create(&tid, NULL, &echo, conn_sock);
        }
    }
    
    close(listen_sock);
    return 0;
}
