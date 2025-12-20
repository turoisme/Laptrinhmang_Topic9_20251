#ifndef ROOM_H
#define ROOM_H
#include "auth.h"
#include "item.h"

//type def
typedef struct Room {
	int roomSock;
	char roomName[20];
	User *host;
	User *guest;
	int bidTime;
	Item *itemList;
	Item *biddingItem;
	struct Room *next;
}Room;

//variable for other files
extern Room *roomList;

//func declaration
void listRoom(Room *list,int padding);
char *takeRoomCommand(int sockfd,char *cmd);

#endif