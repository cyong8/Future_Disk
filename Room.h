#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
#include "Tile.h"

class Wall;

class Tile;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident);
	Ogre::Vector3 getBounds(void);
	Ogre::Real getWidth(void);
	Ogre::Real getHeight(void);
	Ogre::Real getFloorLength(void);
protected:
	Ogre::Real width;
	Ogre::Real height;
	Ogre::Real gapSize;
	Ogre::Real floorLength;
};

#endif // #ifndef __Room_h_
