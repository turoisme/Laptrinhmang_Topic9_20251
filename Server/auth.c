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

void listUser(User *list,int padding){
	while(padding--)printf(" ");
	User *temp=list;
	while(temp){
		printf("User %s has id %i and role %s\n",temp->userName,temp->sockfd,roleToString(temp->role));
		temp=temp->next;
	}
	if(!list)printf("\n");
}

void clearUser(User *list){
	if(!list)return;
	if(list->next!=NULL)clearUser(list->next);
	free(list);
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

char *roleToString(Role role){
	switch(role){
		case ROLE_ADMIN:return "admin";
		case ROLE_HOST:return "host";
		case ROLE_GUEST:return "guest";
		case ROLE_NONE:return "none";
	}
}
//Call when LOGIN, REGISTER,LOGOUT
char *takeAuthCommand(int sockfd,char* cmd) {
	//Parse command, param
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
	if (paramCount==2&&(strlen(param[0]) > 350 || strlen(param[1]) > 30)) return "112\0";
	//Calling command
	if (strcmp(command, "LOGIN") == 0 && paramCount == 2) {
		returnValue=checkAccount(sockfd, param[0], param[1]);
	} else if (strcmp(command, "REGISTER") == 0 && paramCount == 2) {
		returnValue=makeAccount(sockfd, param[0], param[1]);
	} else if (strcmp(command, "LOGOUT") == 0 && paramCount == 0) {
		returnValue=removeAccount(sockfd);
	} else {
		printf("Invalid command entry at auth.c: %s\n",cmd);
		return "000\0";
	}
	//Handling return
	if(!returnValue)strcpy(cmdCode,"000");
	else sprintf(cmdCode,"%3d",returnValue);
	if(strcmp(cmdCode,"100")==0||strcmp(cmdCode,"200")==0) {
		User *newUser=makeUser(sockfd);
		if(!newUser){
			printf("Error at making new user at auth.c\n");
			return "000\0";
		}
		strcpy(newUser->userName,param[0]);
		userList=addUser(userList,newUser);
	}
	return cmdCode;
}