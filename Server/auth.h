

#ifndef AUTH_H
#define AUTH_H

void reset_server_auth();
int is_verified_user(int sockfd,int *account);  //Call this to check if the user is logged in
int set_user_room(int sockfd, int room_id);    //Set room_id for logged in user (-1 to clear)
int get_user_room(int sockfd, int *room_id);   //Get room_id for logged in user
int handle_login(char *message, int sockfd);
int handle_register(char *message, int sockfd);
int handle_logout(char *message, int sockfd);

#endif