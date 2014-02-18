#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Player;

class Disk : public GameObject
{
public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real height, 
		Ogre::Real radius, Ogre::Vector3 iposition);
	void setPlayer(Player* p); //player caught the disk

	Player* heldBy;
};

#endif // #ifndef __Disk_h_
