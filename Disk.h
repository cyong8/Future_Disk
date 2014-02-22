#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Player;

class Disk : public GameObject
{
public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer);
	void updateDiskPosition(Ogre::Vector3 v);
	void updateDiskSNodeOrientation(Ogre::Vector3 v);
};

#endif // #ifndef __Disk_h_
