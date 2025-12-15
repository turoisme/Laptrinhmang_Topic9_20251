// item.c - Item and auction management implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "item.h"

// TODO: Implement item management and auction functions

Item *makeItem(int itemId, float startPrice, float buyNowPrice){
	Item *newItem=(Item *)malloc(sizeof(Item));
	if(!newItem)return NULL;
	newItem->itemId=itemId;
	newItem->startPrice=startPrice;
	newItem->currentPrice=startPrice;
	newItem->buyNowPrice=buyNowPrice;
	newItem->next=NULL;
	return newItem;
}

Item *addItem(Item *list,Item *item){
	if(!item)return list;
	if(!ist){
		list=item;
		return list;
	}
	Item *temp=list;
	while(temp->next!=NULL)temp=temp->next;
	temp->next=newItem;
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
}
Item *findItem(Item *list, int itemId){
	Item *temp=list;
	while(temp){
		if(temp->itemId==itemId)return temp;
		temp=temp->next;
	}
	return NULL;
}
void listItem(Item *list){
	Item *temp=list;
	while(temp){
		print("Item %d is now at %f, buy now at %f",temp->itemId,temp->currentPrice,temp->buyNowPrice);
		temp=temp->next;
	}
}


