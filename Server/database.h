#ifndef DATABASE_H
#define DATABASE_H

#include <mysql/mysql.h>
#include <pthread.h>

// Database connection pool
typedef struct {
    MYSQL *conn;
    int is_available;
    pthread_mutex_t lock;
} DBConnection;

// Database configuration
#define DB_HOST "localhost"
#define DB_USER "auction_user"
#define DB_PASS "auction_pass"
#define DB_NAME "auction_system"
#define DB_POOL_SIZE 100

// Initialize database connection pool
int db_init();
void db_cleanup();

// Get/Release connection from pool
MYSQL* db_get_connection();
void db_release_connection(MYSQL *conn);

// User operations
int db_user_register(const char *username, const char *password);
int db_user_login(const char *username, const char *password, int *user_id);
int db_user_exists(const char *username);

// Room operations
int db_room_create(const char *room_name, int owner_id, int is_private, const char *password);
int db_room_exists(const char *room_name);
int db_room_get_id(const char *room_name);
int db_room_join(int room_id, int user_id);
int db_room_leave(int room_id, int user_id);
int db_room_is_owner(int room_id, int user_id);
int db_user_get_current_room(int user_id);
char* db_room_list_all();

// Item operations
int db_item_create(const char *item_name, int room_id, int owner_id, 
                   double start_price, double buy_now_price);
int db_item_delete(int item_id, int user_id);
int db_item_bid(int item_id, int user_id, double amount);
int db_item_buy(int item_id, int user_id);
char* db_item_list_by_room(int room_id);

// Activity logging
void db_log_activity(int user_id, const char *action, const char *details, const char *ip);

#endif
