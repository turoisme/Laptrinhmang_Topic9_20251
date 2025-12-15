#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"
//#include "database.h"

User *userList=NULL;

User *makeUser(int sockfd) {
	User *user=(User*)malloc(sizeof(User));
	if (!user) return NULL;
	user->sockfd=sockfd;
	user->userName[0]='\0';
	user->curRoom=0;
	user->role=ROLE_NONE;
	user->next=NULL;
	return user;
}

User *addUser(User *list,User *newUser) {
	if (!newUser) return list;
	newUser->next = NULL;
	if(list==NULL) {
		list=newUser;
		return list;
	}
	User *temp=list;
	while(temp->next!=NULL)temp=temp->next;
	temp->next=newUser;
	return list;
}

User *removeUser(User *list,int sockfd) {
	User *temp=list;
	User *prev=NULL;
	while(temp) {
		if(temp->sockfd==sockfd) {
			if(prev)prev->next=temp->next;
			else list=temp->next;
			free(temp);
			return list;
		}
		prev=temp;
		temp=temp->next;
	}
	return list;
}

User *findUser(User *list,int sockfd) {
	User *temp=list;
	while(temp) {
		if(temp->sockfd==sockfd)return temp;
		temp=temp->next;
	}
	return NULL;
}

void listUser(User *list){
	printf("List user\n");
	User *temp=list;
	while(temp){
		printf("User at socket %d has user name %s\n",temp->sockfd,temp->userName);
		temp=temp->next;
	}
}

int checkAccount(int sockfd, char *userAccount, char *password) {
	if (findUser(userList,sockfd)!=NULL) return 213;

	int user_id;
	//int result = db_user_login(userAccount, password, &user_id);
	int result=200;
	switch(result) {
		case 200:
	    case 211:
	        return result;
	    default:
	        return 0;
	}

}

int makeAccount(int sockfd,char *userAccount, char *password) {
	if (findUser(userList,sockfd)!=NULL) return 213;

	//int result = db_user_register(userAccount, password);
	int result=100;
	switch(result) {
		case 100:
		case 111:
			return result;
		default:
			return 0;
	}
}

int removeAccount(int sockfd) {
	if(findUser(userList,sockfd)==NULL)return 000; //ma loi chua log in
	userList=removeUser(userList,sockfd);
	return 000; //ma log out thanh cong
}

char *takeAuthCommand(int sockfd,char* cmd) {
	static char cmdCode[4];
	cmdCode[3]='\0';
	int returnValue;
	int param=0;
	char *command = strtok(cmd, " ");
	char *account = strtok(NULL, " ");
	char *password = strtok(NULL, " ");
	
	if (account){
		param+=1;
	}
	if(password){
		param+=1;
	}

	if (param==2&&(strlen(account) > 350 || strlen(password) > 30)) return "112\0";

	if (strcmp(command, "LOGIN") == 0 && param == 2) {
		returnValue=checkAccount(sockfd, account, password);
	} else if (strcmp(command, "REGISTER") == 0 && param == 2) {
		returnValue=makeAccount(sockfd, account, password);
	} else if (strcmp(command, "LOGOUT") == 0 && param == 0) {
		returnValue=removeAccount(1000); //sockfd
	} else {
		printf("Invalid command entry at auth.c: %s\n",cmd);
		return "000\0";
	}
	if(!returnValue)strcpy(cmdCode,"000");
	else sprintf(cmdCode,"%3d",returnValue);
	if(strcmp(cmdCode,"100")==0||strcmp(cmdCode,"200")==0) {
		User *newUser=makeUser(sockfd);
		if(!newUser){
			printf("Error at making new user at auth.c\n");
			return "000\0";
		}
		strcpy(newUser->userName,account);
		userList=addUser(userList,newUser);
	}
	return cmdCode;
}
