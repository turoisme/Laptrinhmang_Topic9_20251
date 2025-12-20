#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"
//#include "database.h"


void clearRoom(Room *room);
Room *roomList=NULL;

Room *makeRoom(User *host,char *name){
	Room *newRoom=(Room*)malloc(sizeof(Room));
	if(!newRoom)return NULL;
	newRoom->host=host;
	strcpy(newRoom->roomName,name);
	newRoom->guest=NULL;
	newRoom->roomSock=host->sockfd;
	newRoom->bidTime=180;
	newRoom->itemList=NULL;
	newRoom->biddingItem=NULL; 
	host->role=ROLE_HOST;
	host->curRoom=newRoom->roomSock;
	newRoom->next=NULL;
	return newRoom;
}

Room *addRoom(Room *list,Room *newRoom){
	if(!newRoom)return NULL;
	if(list==NULL){
		list=newRoom;
		return list;
	}
	Room *temp=list;
	while(temp->next)temp=temp->next;
	//Save room to database
	temp->next=newRoom;
	return list;
}

Room *removeRoom(Room *list,int sockfd){
	if(!list)return NULL;
	Room *temp=list;
	Room *prev=NULL;
	while(temp){
		if(temp->roomSock==sockfd){
			clearUser(temp->guest);
			clearItem(temp->itemList);
			//Remove from database
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

Room *findRoom(Room *list,int sockfd){
	Room *temp=list;
	while(temp){
		if(temp->roomSock==sockfd)return temp;
		temp=temp->next;
	}
	return NULL;
}

void listRoom(Room *list,int padding){
	while(padding--)printf(" ");
	Room *temp=list;
	while(temp){
		printf("Room %d of host %s\n",temp->roomSock,temp->host->userName);
		User *temp2=temp->guest;
		printf("Guest list:\n");
		listUser(temp->guest,padding+4);
		printf("Item list\n");
		listItem(temp->itemList,padding+4);
		temp=temp->next;
	}
	if(!list)printf("\n");
}

void clearRoom(Room* list){
	if(!list)return;
	if(list->next!=NULL)clearRoom(list->next);
	free(list);
}

int createRoom(int sockfd,char *roomName){
	User *user=findUser(userList,sockfd);
	if(!user){
		printf("User dont exist %i\n",sockfd);
		return 0;
	}
	if(user->curRoom!=0)return 721;
	Room *room=makeRoom(user,roomName);
	roomList=addRoom(roomList,room);
	return 400;
}

void loadRoom(){
	//Load from database to roomList
}

int joinRoom(int sockfd,char *roomId){
	char *end;
	int roomSock=strtol(roomId, &end, 10);
	Room *room=findRoom(roomList,roomSock);
	User *user=findUser(userList,sockfd);
	User *guest=makeUser(sockfd);
	if(!room)return 313;
	if(!user){
		printf("User dont exist %i\n",sockfd);
		return 0;
	}
	if(user->curRoom!=0)return 721;
	if(!guest){
		printf("Error at making guest at room.c\n");
		return 0;
	}
	user->curRoom=room->roomSock;
	user->role=ROLE_GUEST;
	strcpy(guest->userName,user->userName);
	guest->curRoom=user->curRoom;
	guest->role=user->role;
	room->guest=addUser(room->guest,guest);
	return 300;
}

int leaveRoom(int sockfd){
	User *user=findUser(userList,sockfd);
	if(!user){
		printf("User dont exist %i\n",sockfd);
		return 0; //need update
	}
	if(user->curRoom==0)return 000; //not in room code
	Room *room=findRoom(roomList,user->curRoom);
	if(room){
		if(user->role==ROLE_HOST){
			roomList=removeRoom(roomList,room->roomSock);
		}else{
			room->guest=removeUser(room->guest,sockfd);
			user->curRoom=0;		
		}
	}
	user->curRoom=0;
	user->role=ROLE_NONE;
	return 0; //need update
}

void displayNextItem(Room *room){
	if(room->itemList==NULL)return;
	if(room->biddingItem==NULL){
		room->biddingItem=room->itemList;
	}else{
		room->biddingItem=room->biddingItem->next;
	}
	room->biddingItem->status=ST_BIDDING;
}

int placePrice(Room *room,int bidder,int price){
	Item *biddingItem=room->biddingItem;
	if(!biddingItem)return 0; //Need update
	if(price<=biddingItem->currentPrice)return 0;//Need update
	biddingItem->currentPrice=price;
	biddingItem->bidderId=bidder;
	//Logic for when currentPrice>buyNowPrice
	return 0; //Need update
}

int buyNow(Room *room,int bidder){
	Item *biddingItem=room->biddingItem;
	if(!biddingItem)return 0; //Need update
	biddingItem->currentPrice=biddingItem->buyNowPrice;
	biddingItem->bidderId=bidder;
	biddingItem->status=ST_SOLD;
}
//Call when CREATE_ROOM JOIN_ROOM LEAVE_ROOM
char *takeRoomCommand(int sockfd,char* cmd) {
	
	static char cmdCode[4];
	cmdCode[3]='\0';
	int returnValue;
	char *token;
	char *command = strtok(cmd, " ");
	int paramCount=0;
	char param[10][350];
	while((token=strtok(NULL, " "))!=NULL){
		strcpy(param[paramCount],token);
		paramCount++;
	}
	
	if (strcmp(command,"CREATE_ROOM")==0 && paramCount==1) {
		returnValue=createRoom(sockfd,param[0]);
	} else if (strcmp(command,"JOIN_ROOM")==0 && paramCount==1) {
		returnValue=joinRoom(sockfd,param[0]);
	} else if (strcmp(command,"LEAVE_ROOM")==0 && paramCount==0) {
		returnValue=leaveRoom(sockfd);
	} else {
		printf("Invalid command entry at room.c: %s\n",cmd);
		return "000\0";
	}
	if(!returnValue)strcpy(cmdCode,"000");
	else sprintf(cmdCode,"%3d",returnValue);
	return cmdCode;
}