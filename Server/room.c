// room.c - Auction room management implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "room.h"

// Implement room management functions

Room *roomList=NULL;

Room *makeRoom(User *host){
	Room *newRoom=(Room*)malloc(sizeof(Room));
	if(!newRoom)return NULL;
	newRoom->host=host;
	newRoom->guest=NULL;
	newRoom->roomSock=host->sockfd;
	//newRoom->itemList=NULL   //activate when itemList is finished
	host->role=ROLE_HOST;
	host->curRoom=newRoom->roomSock;
	newRoom->next=NULL;
	return newRoom;
}

void addRoom(Room *list,Room *newRoom){
	if(!newRoom)return;
	if(list==NULL){
		list=newRoom;
		return;
	}
	Room *temp=list;
	while(temp->next)temp=temp->next;
	temp->next=newRoom;
}

void removeRoom(Room *list,int sockfd){
	if(!list)return;
	Room *temp=list;
	Room *prev=NULL;
	while(temp){
		if(temp->roomSock==sockfd){
			if(!prev)list=temp->next;
			else prev->next=temp->next;
			free(temp);
			return;
		}
		prev=temp;
		temp=temp->next;
	}
}

Room *findRoom(Room *list,int sockfd){
	Room *temp=list;
	while(temp){
		if(temp->roomSock==sockfd)return temp;
		temp=temp->next;
	}
	return NULL;
}

void listRoom(Room *list){
	Room *temp=list;
	while(temp){
		printf("Room %d of host %s\n",temp->roomSock,temp->host->userName);
		temp=temp->next;
	}
}

int createRoom(int sockfd){
	User *user=findUser(userList,sockfd);
	if(!user){
		printf("User dont exist %i\n",sockfd);
		return 0;
	}
	if(user->curRoom!=0)return 721;
	Room *room=makeRoom(user);
	addRoom(roomList,room);
	return 400;
}

int joinRoom(int sockfd,int roomSock){
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
	room->guest=(room->guest,guest);
	return 300;
}

int leaveRoom(int sockfd){
	User *user=findUser(userList,sockfd);
	if(!user){
		printf("User dont exist %i\n",sockfd);
		return 0;
	}
	if(user->curRoom==0)return 000; //not in room code
	Room *room=findRoom(roomList,user->curRoom);
	if(room){
		removeUser(room->guest,sockfd);
	}
	user->curRoom=0;
}
