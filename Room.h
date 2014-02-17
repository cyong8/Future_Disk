#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
class Wall;

class Room
{
public:
	Room();
	vector<Wall> wallList;

protected:
	void addWall(Wall w);
};

#endif // #ifndef __Room_h_
