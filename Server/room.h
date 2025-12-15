#ifndef ROOM_H
#define ROOM_H
#include "auth.h"

// TODO: Define Room structure
// - Create auction room (CREATE_ROOM)
// - List rooms (LIST_ROOMS)
// - Join room (JOIN_ROOM)
// - Control room access
// - Manage room members

typedef struct Room {
	int roomSock;
	User *host;
	User *guest;
	//Item *itemList;   //is for item in room, activate later
	struct Room *next;
}Room;

extern Room *roomList;

#endif
