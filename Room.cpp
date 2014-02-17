#include "Room.h"

Room::Room()
{
	wallList = vector<Wall>();
}

void Room::addWall(Wall w)
{
	((*this).wallList).push_back(w);
}