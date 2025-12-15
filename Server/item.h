// item.h - Auction item management
// Create, delete, list items, handle bidding

#ifndef ITEM_H
#define ITEM_H
#include "room.h"

// TODO: Define Item structure
// - Create item (CREATE_ITEM)
// - Delete item (DELETE_ITEM)
// - List items (LIST_ITEMS)
// - Bid (BID)
// - Buy now (BUY)
// - Auction countdown timer (3 minutes)
// - Notify clients in room

typedef struct Item{
	int itemId;
	float startPrice;
	float currentPrice;
	float buyNowPrice;
	struct Item *next;
}Item;

Item *makeItem(int itemId,float startPrice, float buyNowPrice);
Item *addItem(Item *list,Item *item);
Item *removeItem(Item *list,int itemId);
Item *findItem(Item *list,int itemId);
void listItem(Item *list);

#endif
