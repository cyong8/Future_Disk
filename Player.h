#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

class Disk;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setHolding(void);
	void update(float elapsedTime);
	void attachDisk(Disk* d);
	bool checkHolding(void);
	void updateDiskPosition(Ogre::Vector3 v);
	Disk* getPlayerDisk(void);

protected:
	bool isHolding;		
	Disk* player_disk;
	Ogre::Vector3 dimensions;
};

#endif // #ifndef __Player_h_
