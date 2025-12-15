

#ifndef AUTH_H
#define AUTH_H

#include <stddef.h>

#define DATA_SIZE 1024
#define USERNAME_LEN 350

typedef enum {
	ROLE_ADMIN,
	ROLE_HOST,
	ROLE_GUEST,
	ROLE_NONE
}Role;

char *takeAuthCommand(int sockfd, char* cmd);

typedef struct User{
	int sockfd;
	char userName[USERNAME_LEN];
	int curRoom;
	Role role;
	struct User *next;
}User;

extern User *userList;

User *makeUser(int sockfd);
User *addUser(User *list,User *user);
User *removeUser(User *list,int sockfd);
User *findUser(User *list,int sockfd);
void listUser(User *list);

#endif
