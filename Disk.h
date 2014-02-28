#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Player;

class Disk : public GameObject
{
protected:
	bool offWallRotation;
	Ogre::Vector3 diskDirection;
public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer);
	void setRotateOffWall();
	void rotateOffWall();
	bool checkOffWallRotation();
};

#endif // #ifndef __Disk_h_
