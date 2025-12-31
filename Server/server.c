#include "server.h"
#include "socket_handler.h"
#include "thread_pool.h"
#include "database.h"
#include "auction_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "auth.h"
// Cleanup on exit
void cleanup_handler(int sig) {
    printf("\nShutting down server...\n");
    stop_auction_timer();
    db_cleanup();
    exit(0);
}

int main(int argc, char *argv[]) {
    int listen_sock, *conn_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    pthread_t tid;
    
    // Setup signal handlers
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
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
    
    // Initialize MySQL database
    printf("Connecting to MySQL database...\n");
    if (db_init() != 0) {
        fprintf(stderr, "Failed to initialize database!\n");
        fprintf(stderr, "Please check:\n");
        fprintf(stderr, "1. MySQL is running: sudo service mysql start\n");
        fprintf(stderr, "2. Database exists: mysql -u root -p < schema.sql\n");
        fprintf(stderr, "3. Credentials in database.h are correct\n");
        exit(EXIT_FAILURE);
    }
    printf("Database connected successfully!\n");
    
    // Start auction timer thread
    start_auction_timer();
    
    // Step 1: Construct a TCP socket to listen connection request
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error: ");
        db_cleanup();
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
    reset_server_auth();
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
