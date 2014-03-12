#ifndef __Wall_h_
#define __Wall_h_

#include "GameObject.h"

class Wall : public GameObject
{
public:
	Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Plane op, Ogre::Vector3 norm);
};

#endif // #ifndef __Wall_h_
