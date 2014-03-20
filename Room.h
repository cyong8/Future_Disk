#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
class Wall;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator);
	Ogre::Real getWidth(void);
	Ogre::Real getHeight(void);
	Ogre::Real getFloorLength(void);
protected:
	Ogre::Real width;
	Ogre::Real height;
	Ogre::Real floorLength;
};

#endif // #ifndef __Room_h_
