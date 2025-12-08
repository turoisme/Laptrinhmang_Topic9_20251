#include "socket_handler.h"
// Construct a TCP socket to listen connection request
int create_server_socket(int port) {
    int listen_sock;
    struct sockaddr_in server_addr;
    
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() error: ");
        exit(EXIT_FAILURE);
    }
    
    // Bind address to socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error: ");
        exit(EXIT_FAILURE);
    }
    
    // Listen request from client
    if (listen(listen_sock, BACKLOG) == -1) {
        perror("listen() error: ");
        exit(EXIT_FAILURE);
    }
    
    printf("Server started at port %d\n", port);
    return listen_sock;
}

// Accept client connection
int accept_client(int listen_sock) {
    struct sockaddr_in client_addr;
    int sin_size = sizeof(struct sockaddr_in);
    int conn_sock;
    
    if ((conn_sock = accept(listen_sock, (struct sockaddr*)&client_addr, (socklen_t*)&sin_size)) == -1) {
        perror("accept() error: ");
        exit(EXIT_FAILURE);
    }
    
    // Get client info
    char client_ip[INET_ADDRSTRLEN];
    int client_port;
    
    if (inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop() error:");
    } else {
        client_port = ntohs(client_addr.sin_port);
        printf("You got a connection from %s:%d\n", client_ip, client_port);
    }
    
    return conn_sock;
}

// Stream processing function - send message with delimiter
int send_message(int sockfd, const char *message) {
    size_t msg_len = strlen(message) + strlen(DELIMITER);
    char *full_message = malloc(msg_len + 1);
    if (!full_message) return -1;
    
    snprintf(full_message, msg_len + 1, "%s%s", message, DELIMITER);
    
    int total_sent = 0;
    int len = strlen(full_message);
    while (total_sent < len) {
        int sent = send(sockfd, full_message + total_sent, len - total_sent, 0);
        if (sent <= 0) {
            free(full_message);
            return -1;
        }
        total_sent += sent;
    }
    free(full_message);
    return total_sent;
}

// Process multiple messages in 1 buffer - receive message until delimiter
int recv_message(int sockfd, char **message, ConnectionBuffer *conn_buf) {
    char *mess = malloc(1);
    if (!mess) return -1;
    mess[0] = '\0';
    int mess_len = 0;
    
    // Check if the buffer has a complete message
    if (conn_buf->buffer_len > 0) {
        char *delim = strstr(conn_buf->buffer, DELIMITER);
        if (delim) {
            int msg_len = delim - conn_buf->buffer;
            mess = realloc(mess, msg_len + 1);
            if (!mess) return -1;
            
            memcpy(mess, conn_buf->buffer, msg_len);
            mess[msg_len] = '\0';
           
            int remaining = conn_buf->buffer_len - msg_len - strlen(DELIMITER);
            if (remaining > 0) {
                memmove(conn_buf->buffer, delim + strlen(DELIMITER), remaining);
            }
            conn_buf->buffer_len = remaining;
            
            *message = mess;
            return msg_len;
        }
        
        // Not enough messages, copy to mess
        mess = realloc(mess, conn_buf->buffer_len + 1);
        if (!mess) return -1;
        memcpy(mess, conn_buf->buffer, conn_buf->buffer_len);
        mess[conn_buf->buffer_len] = '\0';
        mess_len = conn_buf->buffer_len;
        conn_buf->buffer_len = 0;
    }
    
    while (1) {
        char buf[BUFF_SIZE];
        memset(buf, 0, BUFF_SIZE);
        int received = recv(sockfd, buf, BUFF_SIZE - 1, 0);
        if (received <= 0) {
            free(mess);
            return -1;
        }
        buf[received] = '\0';
        
        // Find delimiter
        char *delim = strstr(buf, DELIMITER);
        if (delim) {
            int msg_part_len = delim - buf;
            mess = realloc(mess, mess_len + msg_part_len + 1);
            if (!mess) return -1;
            memcpy(mess + mess_len, buf, msg_part_len);
            mess_len += msg_part_len;
            mess[mess_len] = '\0';
            
            // Put the rest into buffer
            int remaining = received - msg_part_len - strlen(DELIMITER);
            if (remaining > 0) {
                memcpy(conn_buf->buffer, delim + strlen(DELIMITER), remaining);
                conn_buf->buffer_len = remaining;
            }
            
            *message = mess;
            return mess_len;
        } else {
            // No delimiter yet, continue merging
            mess = realloc(mess, mess_len + received + 1);
            if (!mess) return -1;
            memcpy(mess + mess_len, buf, received);
            mess_len += received;
            mess[mess_len] = '\0';
        }
    }
}

// Close socket connection
void close_socket(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
    }
}
