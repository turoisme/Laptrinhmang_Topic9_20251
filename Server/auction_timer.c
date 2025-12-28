#include "auction_timer.h"
#include "database.h"
#include "protocol.h"
#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mysql/mysql.h>

static pthread_t timer_thread;
static int timer_running = 0;

void* auction_timer_worker(void* arg) {
    (void)arg;
    printf("Auction timer thread started\n");
    
    while (timer_running) {
        sleep(10);
        
        MYSQL *conn = db_get_connection();
        if (!conn) continue;
        
        // Get all active items (not sold, not expired)
        char query[1024];
        snprintf(query, sizeof(query),
                 "SELECT i.item_id, i.item_name, i.room_id, "
                 "TIMESTAMPDIFF(SECOND, NOW(), i.expires_at) as remaining, "
                 "i.current_bidder_id, i.current_price, i.buy_now_price "
                 "FROM items i "
                 "WHERE i.is_sold = 0 AND i.expires_at > NOW() "
                 "ORDER BY i.expires_at ASC");
        
        if (mysql_query(conn, query)) {
            db_release_connection(conn);
            continue;
        }
        
        MYSQL_RES *result = mysql_store_result(conn);
        if (!result) {
            db_release_connection(conn);
            continue;
        }
        
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            int item_id = atoi(row[0]);
            char *item_name = row[1];
            int room_id = atoi(row[2]);
            int remaining = atoi(row[3]);
            int current_bidder_id = row[4] ? atoi(row[4]) : 0;
            double current_price = atof(row[5]);
            
            // Send countdown for items under 180 seconds (every check)
            if (remaining > 0 && remaining <= 180) {
                char msg[256];
                if (current_bidder_id > 0) {
                    snprintf(msg, sizeof(msg), "%s: %ds left! Current bid: $%.2f", 
                            item_name, remaining, current_price);
                } else {
                    snprintf(msg, sizeof(msg), "%s: %ds left! No bids yet (Start: $%.2f)", 
                            item_name, remaining, current_price);
                }
                broadcast_to_room(room_id, -1, NOTIFY_COUNTDOWN, msg);
                printf("Countdown: %s - %ds left\n", item_name, remaining);
            }
            
        }
        
        mysql_free_result(result);
        db_release_connection(conn);
        
        // Check for expired items (auction ended)
        conn = db_get_connection();
        if (!conn) continue;
        
        snprintf(query, sizeof(query),
                 "SELECT i.item_id, i.item_name, i.room_id, "
                 "i.current_bidder_id, i.current_price, u.username "
                 "FROM items i "
                 "LEFT JOIN users u ON i.current_bidder_id = u.user_id "
                 "WHERE i.is_sold = 0 AND i.expires_at <= NOW()");
        
        if (mysql_query(conn, query)) {
            db_release_connection(conn);
            continue;
        }
        
        result = mysql_store_result(conn);
        if (!result) {
            db_release_connection(conn);
            continue;
        }
        
        while ((row = mysql_fetch_row(result))) {
            int item_id = atoi(row[0]);
            char *item_name = row[1];
            int room_id = atoi(row[2]);
            int winner_id = row[3] ? atoi(row[3]) : 0;
            double final_price = atof(row[4]);
            char *winner_name = row[5];
            
            // Mark as sold if there was a bidder
            if (winner_id > 0) {
                char update[512];
                snprintf(update, sizeof(update),
                         "UPDATE items SET is_sold = 1 WHERE item_id = %d",
                         item_id);
                mysql_query(conn, update);
                
                char msg[256];
                snprintf(msg, sizeof(msg), 
                        "AUCTION ENDED: %s sold to %s for $%.2f!", 
                        item_name, winner_name, final_price);
                broadcast_to_room(room_id, -1, NOTIFY_AUCTION_END, msg);
                printf("Auction ended: %s -> %s ($%.2f)\n", item_name, winner_name, final_price);
            } else {
                // No bidders - delete the item
                char delete_query[512];
                snprintf(delete_query, sizeof(delete_query),
                         "DELETE FROM items WHERE item_id = %d", item_id);
                mysql_query(conn, delete_query);
                
                char msg[256];
                snprintf(msg, sizeof(msg), 
                        "AUCTION ENDED: %s expired with no bids (removed)", 
                        item_name);
                broadcast_to_room(room_id, -1, NOTIFY_AUCTION_END, msg);
                printf("Auction expired: %s (no bids, deleted)\n", item_name);
            }
        }
        
        mysql_free_result(result);
        db_release_connection(conn);
    }
    
    printf("Auction timer thread stopped\n");
    return NULL;
}

void start_auction_timer() {
    if (timer_running) {
        printf("Auction timer already running\n");
        return;
    }
    
    timer_running = 1;
    
    if (pthread_create(&timer_thread, NULL, auction_timer_worker, NULL) != 0) {
        perror("Failed to create auction timer thread");
        timer_running = 0;
        return;
    }
    
    pthread_detach(timer_thread);
    printf("Auction timer started successfully\n");
}

void stop_auction_timer() {
    if (timer_running) {
        timer_running = 0;
        printf("Stopping auction timer...\n");
    }
}
