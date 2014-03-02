#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Player;

class Disk : public GameObject
{
protected:
	bool offWallRotation;
	btVector3 thrownVelocity;
public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer);
	void setRotateOffWall(void);
	void resetRotateOffWall(void);
	void rotateOffWall(void);
	bool checkOffWallRotation(void);
	void setThrownVelocity(btVector3 v);
	btVector3 getThrownVelocity(void);
	Ogre::Vector3 diskDirection;

};

#endif // #ifndef __Disk_h_
