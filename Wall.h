#ifndef __Wall_h_
#define __Wall_h_

#include "GameObject.h"
class GameObject;

class Wall : public GameObject
{
public:
	Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
};

#endif // #ifndef __Wall_h_
