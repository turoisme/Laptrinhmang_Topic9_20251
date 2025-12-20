

#ifndef AUTH_H
#define AUTH_H

#include <stddef.h>

#define DATA_SIZE 1024
#define USERNAME_LEN 350

//type def
typedef enum {
	ROLE_ADMIN,
	ROLE_HOST,
	ROLE_GUEST,
	ROLE_NONE
}Role;

typedef struct User{
	int sockfd;
	char userName[USERNAME_LEN];
	int curRoom;
	Role role;
	struct User *next;
}User;

//variable for other files
extern User *userList;

//func declaration
User *makeUser(int sockfd);
User *addUser(User *list,User *user);
User *removeUser(User *list,int sockfd);
User *findUser(User *list,int sockfd);
void listUser(User *list,int padding);
void clearUser(User *list);
char *roleToString(Role role);
char *takeAuthCommand(int sockfd, char* cmd);

#endif