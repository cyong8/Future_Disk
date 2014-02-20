#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setHolding();
	void update(float elapsedTime);
	void setPlayerHit(string typeOfObjectCollided);

protected:
	bool isHolding;
	bool hit_disk;
	bool hit_wall;
};

#endif // #ifndef __Player_h_
