#include "Room.h"

Room::Room()
{
	wallList = vector<Wall>();  // might be unnecessary
}

void Room::addWall(Wall w)
{
	((*this).wallList).push_back(w);
}