#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include "socket_handler.h"

// Client handler thread function
void *echo(void *arg);

#endif
