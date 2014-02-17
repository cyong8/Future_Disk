#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"
#include "PlayerCamera.h"
class GameObject;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
	void setHolding();

protected:
	bool isHolding;
};

#endif // #ifndef __Player_h_
