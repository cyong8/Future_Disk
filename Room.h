#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
class Wall;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator);
	vector<Wall> wallList;

protected:
	void addWall(Wall *w);
	void addWalls(vector<Wall> wallList, Ogre::SceneManager *mSceneMgr, Simulator *game_simulator);
};

#endif // #ifndef __Room_h_
