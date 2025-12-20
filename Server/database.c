#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Connection pool
static DBConnection db_pool[DB_POOL_SIZE];
static pthread_mutex_t pool_lock = PTHREAD_MUTEX_INITIALIZER;

// Initialize database connection pool
int db_init() {

    for (int i = 0; i < DB_POOL_SIZE; i++) {
        db_pool[i].conn = mysql_init(NULL);
        if (!db_pool[i].conn) {
            return -1;
        }
        
        if (!mysql_real_connect(db_pool[i].conn, DB_HOST, DB_USER, DB_PASS,
                                DB_NAME, 0, NULL, 0)) {
            return -1;
        }
        
        db_pool[i].is_available = 1;
        pthread_mutex_init(&db_pool[i].lock, NULL);
    }
    
    return 0;
}

// Cleanup database
void db_cleanup() {
    for (int i = 0; i < DB_POOL_SIZE; i++) {
        if (db_pool[i].conn) {
            mysql_close(db_pool[i].conn);
            pthread_mutex_destroy(&db_pool[i].lock);
        }
    }
}

// Get available connection from pool
MYSQL* db_get_connection() {
    pthread_mutex_lock(&pool_lock);
    
    for (int i = 0; i < DB_POOL_SIZE; i++) {
        pthread_mutex_lock(&db_pool[i].lock);
        if (db_pool[i].is_available) {
            db_pool[i].is_available = 0;
            pthread_mutex_unlock(&db_pool[i].lock);
            pthread_mutex_unlock(&pool_lock);
            return db_pool[i].conn;
        }
        pthread_mutex_unlock(&db_pool[i].lock);
    }
    
    pthread_mutex_unlock(&pool_lock);
    return NULL;
}

// Release connection back to pool
void db_release_connection(MYSQL *conn) {
    pthread_mutex_lock(&pool_lock);
    
    for (int i = 0; i < DB_POOL_SIZE; i++) {
        if (db_pool[i].conn == conn) {
            pthread_mutex_lock(&db_pool[i].lock);
            db_pool[i].is_available = 1;
            pthread_mutex_unlock(&db_pool[i].lock);
            break;
        }
    }
    
    pthread_mutex_unlock(&pool_lock);
}

// User registration
int db_user_register(const char *username, const char *password) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO users (username, password) VALUES ('%s', '%s')",
             username, password);
    
    int result = mysql_query(conn, query);
    
    if (result != 0) {
        if (mysql_errno(conn) == 1062) {
            db_release_connection(conn);
            return 111;
        }
        fprintf(stderr, "Register failed: %s\n", mysql_error(conn));
        db_release_connection(conn);
        return -1;
    }
    
    db_release_connection(conn);
    return 100;
}

// User login
int db_user_login(const char *username, const char *password, int *user_id) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT user_id FROM users WHERE username='%s' AND password='%s' AND is_active=1",
             username, password);
    
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Login query failed: %s\n", mysql_error(conn));
        db_release_connection(conn);
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        db_release_connection(conn);
        return -1;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    
    if (row) {
        *user_id = atoi(row[0]);
        mysql_free_result(result);
        db_release_connection(conn);
        return 200;
    }
    
    mysql_free_result(result);
    db_release_connection(conn);
    return 211;
}

// User exists check
int db_user_exists(const char *username) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM users WHERE username='%s'",
             username);
    
    if (mysql_query(conn, query)) {
        db_release_connection(conn);
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int exists = atoi(row[0]) > 0;
    
    mysql_free_result(result);
    db_release_connection(conn);
    
    return exists;
}

// Create room
int db_room_create(const char *room_name, int owner_id, int is_private, const char *password) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    if (is_private && password) {
        snprintf(query, sizeof(query),
                 "INSERT INTO rooms (room_name, owner_id, is_private, password) VALUES ('%s', %d, 1, '%s')",
                 room_name, owner_id, password);
    } else {
        snprintf(query, sizeof(query),
                 "INSERT INTO rooms (room_name, owner_id) VALUES ('%s', %d)",
                 room_name, owner_id);
    }
    
    if (mysql_query(conn, query)) {
        if (mysql_errno(conn) == 1062) {
            db_release_connection(conn);
            return -2;
        }
        fprintf(stderr, "Create room failed: %s\n", mysql_error(conn));
        db_release_connection(conn);
        return -1;
    }
    
    int room_id = mysql_insert_id(conn);
    db_release_connection(conn);
    
    return room_id;
}

// Get room ID
int db_room_get_id(const char *room_name) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    snprintf(query, sizeof(query),
             "SELECT room_id FROM rooms WHERE room_name='%s' AND is_active=1",
             room_name);
    
    if (mysql_query(conn, query)) {
        db_release_connection(conn);
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    
    int room_id = row ? atoi(row[0]) : -1;
    
    mysql_free_result(result);
    db_release_connection(conn);
    
    return room_id;
}

// Join room
int db_room_join(int room_id, int user_id) {
    MYSQL *conn = db_get_connection();
    if (!conn) return -1;
    
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO room_members (room_id, user_id) VALUES (%d, %d)",
             room_id, user_id);
    
    int result = mysql_query(conn, query);
    db_release_connection(conn);
    
    return result == 0 ? 0 : -1;
}

// Log activity
void db_log_activity(int user_id, const char *action, const char *details, const char *ip) {
    MYSQL *conn = db_get_connection();
    if (!conn) return;
    
    char query[1024];
    snprintf(query, sizeof(query),
             "INSERT INTO activity_logs (user_id, action, details, ip_address) VALUES (%d, '%s', '%s', '%s')",
             user_id, action, details ? details : "", ip ? ip : "");
    
    mysql_query(conn, query);
    db_release_connection(conn);
}
