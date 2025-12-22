

#ifndef AUTH_H
#define AUTH_H

int handle_login(char *message, int sockfd);
int handle_register(char *message, int sockfd);
int handle_logout(char *message, int sockfd);

#endif