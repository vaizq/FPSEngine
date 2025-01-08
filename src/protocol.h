#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "engine/Transform.h"

struct Player {
	int id;
	Transform transform;
};

struct GameUpdate {
	int myId;
	Player players[10];
	size_t numEnemies;
};


#endif
