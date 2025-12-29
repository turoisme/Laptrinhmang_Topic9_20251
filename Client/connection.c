#include "connection.h"
int connect_to_server(ClientConnection *conn, const char *ip, int port) {
    // Create socket
    conn->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn->sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    
    memset(&conn->server_addr, 0, sizeof(conn->server_addr));
    conn->server_addr.sin_family = AF_INET;
    conn->server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &conn->server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(conn->sockfd);
        return -1;
    }
    
    if (connect(conn->sockfd, (struct sockaddr*)&conn->server_addr, 
                sizeof(conn->server_addr)) < 0) {
        perror("Connection failed");
        close(conn->sockfd);
        return -1;
    }
    
    // Initialize connection buffer
    memset(&conn->conn_buf, 0, sizeof(ConnectionBuffer));
    conn->is_connected = 1;
    
    printf("Connected to server %s:%d\n", ip, port);
    return 0;
}

void disconnect_from_server(ClientConnection *conn) {
    if (conn->is_connected) {
        close(conn->sockfd);
        conn->is_connected = 0;
        printf("Disconnected from server\n");
    }
}
int send_command(ClientConnection *conn, const char *command) {
    if (!conn->is_connected) {
        fprintf(stderr, "Not connected to server\n");
        return -1;
    }
    
    char message[BUFF_SIZE];
    int msg_len = snprintf(message, BUFF_SIZE, "%s%s", command, DELIMITER);
    int total_sent = 0;
    while (total_sent < msg_len) {
        int sent = send(conn->sockfd, message + total_sent, 
                       msg_len - total_sent, 0);
        if (sent <= 0) {
            perror("Send failed");
            conn->is_connected = 0;
            return -1;
        }
        total_sent += sent;
    }
    
    return total_sent;
}

int receive_response(ClientConnection *conn, char **response) {
    if (!conn->is_connected) {
        fprintf(stderr, "Not connected to server\n");
        return -1;
    }
    
    char *mess = malloc(1);
    if (!mess) return -1;
    mess[0] = '\0';
    int mess_len = 0;
    
    if (conn->conn_buf.buffer_len > 0) {
        char *delim = strstr(conn->conn_buf.buffer, DELIMITER);
        if (delim) {
            int msg_len = delim - conn->conn_buf.buffer;
            mess = realloc(mess, msg_len + 1);
            if (!mess) return -1;
            
            memcpy(mess, conn->conn_buf.buffer, msg_len);
            mess[msg_len] = '\0';
            
            int remaining = conn->conn_buf.buffer_len - msg_len - strlen(DELIMITER);
            if (remaining > 0) {
                memmove(conn->conn_buf.buffer, delim + strlen(DELIMITER), remaining);
            }
            conn->conn_buf.buffer_len = remaining;
            
            *response = mess;
            return msg_len;
        }
        
        // Copy existing buffer to mess
        mess = realloc(mess, conn->conn_buf.buffer_len + 1);
        if (!mess) return -1;
        memcpy(mess, conn->conn_buf.buffer, conn->conn_buf.buffer_len);
        mess[conn->conn_buf.buffer_len] = '\0';
        mess_len = conn->conn_buf.buffer_len;
        conn->conn_buf.buffer_len = 0;
    }
    
    // Receive from socket
    while (1) {
        char buf[BUFF_SIZE];
        memset(buf, 0, BUFF_SIZE);
        int received = recv(conn->sockfd, buf, BUFF_SIZE - 1, 0);
        
        if (received <= 0) {
            free(mess);
            conn->is_connected = 0;
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
            
            // Save remainder
            int remaining = received - msg_part_len - strlen(DELIMITER);
            if (remaining > 0) {
                memcpy(conn->conn_buf.buffer, delim + strlen(DELIMITER), remaining);
                conn->conn_buf.buffer_len = remaining;
            }
            
            *response = mess;
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

// Check if connected
int is_connected(ClientConnection *conn) {
    return conn->is_connected;
}
