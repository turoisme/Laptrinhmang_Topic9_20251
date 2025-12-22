// item.h - Auction item management
// Create, delete, list items, handle bidding

#ifndef ITEM_H
#define ITEM_H

int handle_create_item(char *message, int sockfd);
int handle_delete_item(char *message, int sockfd);
int handle_list_items(int sockfd);

#endif