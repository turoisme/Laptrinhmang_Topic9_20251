#ifndef ROOM_H
#define ROOM_H

int handle_create_room(char *message, int sockfd);
int handle_join_room(char *message, int sockfd);
int handle_list_rooms(int sockfd);
int handle_leave_room(char *message, int sockfd);
int handle_bid(char *message, int sockfd);
int handle_buy(char *message, int sockfd);

#endif