#ifndef __Target_h_
#define __Target_h_

#include "GameObject.h"

class Target : public GameObject
{
public:
	Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setTargetHit();
	bool isHit(); 

protected:
	bool hit; // set true on hit, set false on placement
};

#endif // #ifndef __Target_h_