// item.c - Item and auction management implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "item.h"
//#include "database.h"

// TODO: Implement item management and auction functions

Item *makeItem(int itemId, float startPrice, float buyNowPrice){
	Item *newItem=(Item *)malloc(sizeof(Item));
	if(!newItem)return NULL;
	//Need logic for item id
	newItem->status=ST_INQUEUE;
	newItem->bidderId=0;
	newItem->itemId=itemId;
	newItem->startPrice=startPrice;
	newItem->currentPrice=startPrice;
	newItem->buyNowPrice=buyNowPrice;
	newItem->next=NULL;
	return newItem;
}

Item *addItem(Item *list,Item *item){
	if(!item)return list;
	if(!list){
		list=item;
		return list;
	}
	Item *temp=list;
	while(temp->next!=NULL)temp=temp->next;
	temp->next=item;
	return list;
}

Item *removeItem(Item *list,int itemId){
	Item *temp=list;
	Item *prev=NULL;
	while(temp){
		if(temp->itemId==itemId){
			if(!prev)list=temp->next;
			else prev->next=temp->next;
			free(temp);
			return list;
		}
		prev=temp;
		temp=temp->next;
	}
	return list;
}
Item *findItem(Item *list, int itemId){
	Item *temp=list;
	while(temp){
		if(temp->itemId==itemId)return temp;
		temp=temp->next;
	}
	return NULL;
}
void listItem(Item *list,int padding){
	while(padding--)printf(" ");
	Item *temp=list;
	while(temp){
		printf("Item %d is now at %f, buy now at %f. Item is %s",
		temp->itemId,temp->currentPrice,temp->buyNowPrice,statusToString(temp->status));
		temp=temp->next;
	}
	if(!list)printf("\n");
}

void clearItem(Item *list){
	if(!list)return;
	if(list->next!=NULL)clearItem(list->next);
	free(list);
}

char *statusToString(Status status){
	switch(status){
		case ST_INQUEUE:return "in queue";
		case ST_BIDDING:return "bidding";
		case ST_SOLD:return "sold";
	}
}
