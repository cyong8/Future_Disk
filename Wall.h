#ifndef __Wall_h_
#define __Wall_h_

#include "GameObject.h"

class Wall : public GameObject
{
public:
	Wall(Ogre::String nym, Ogre::String planeNym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 norm, Ogre::Vector3 position, Ogre::Vector3 dimensions);
	Ogre::Vector3 dims;
	Ogre::Vector3 pos;
	Ogre::Vector3 normal;
	Ogre::Vector3 getNormal(void);
	Ogre::Vector3 getCenter(void);
};

#endif // #ifndef __Wall_h_
