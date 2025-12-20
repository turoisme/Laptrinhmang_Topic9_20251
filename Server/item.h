// item.h - Auction item management
// Create, delete, list items, handle bidding

#ifndef ITEM_H
#define ITEM_H

// TODO: Define Item structure
// - Create item (CREATE_ITEM)
// - Delete item (DELETE_ITEM)
// - List items (LIST_ITEMS)
// - Bid (BID)
// - Buy now (BUY)
// - Auction countdown timer (3 minutes)
// - Notify clients in room


//type def
typedef enum {
	ST_INQUEUE,
	ST_BIDDING,
	ST_SOLD
}Status;

typedef struct Item{
	int itemId;
	int bidderId;
	Status status;
	float startPrice;
	float currentPrice;
	float buyNowPrice;
	struct Item *next;
}Item;

//variable for other files

//func declaration
Item *makeItem(int itemId,float startPrice, float buyNowPrice);
Item *addItem(Item *list,Item *item);
Item *removeItem(Item *list,int itemId);
Item *findItem(Item *list,int itemId);
void listItem(Item *list,int padding);
void clearItem(Item *list);
char *statusToString(Status status);

#endif