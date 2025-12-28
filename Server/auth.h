

#ifndef AUTH_H
#define AUTH_H

// External declarations for session management
extern int verified[1024];
extern int verify_account[1024];

void reset_server_auth();
int is_verified_user(int sockfd,int *account);  //Call this to check if the user is logged in
int handle_login(char *message, int sockfd);
int handle_register(char *message, int sockfd);
int handle_logout(char *message, int sockfd);

#endif