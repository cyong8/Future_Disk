#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real height, Ogre::Real radius, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setHolding();
	void update(float elapsedTime);

protected:
	bool isHolding;
};

#endif // #ifndef __Player_h_
