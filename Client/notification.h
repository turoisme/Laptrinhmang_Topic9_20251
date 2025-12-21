#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <pthread.h>
#include "connection.h"

// Notification codes
#define NOTIFY_BID 1000
#define NOTIFY_BUY 1001
#define NOTIFY_ITEM_CREATED 1002
#define NOTIFY_ITEM_DELETED 1003
#define NOTIFY_COUNTDOWN 1004
#define NOTIFY_AUCTION_END 1005
#define NOTIFY_USER_JOIN 1006
#define NOTIFY_USER_LEAVE 1007

// Thread control
void start_notification_thread(ClientConnection *conn);
void stop_notification_thread();
int is_notification_running();

#endif
