#ifndef __Target_h_
#define __Target_h_

#include "GameObject.h"

#define NUM_OF_TARGETS  7

class Room;

class Target : public GameObject
{
public:
	Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Room* rm);
	void targetHit();
	void resetHit();
	bool isHit(); 
	Ogre::Vector3 getDimensions() { return dims; }

protected:
	Ogre::Vector3 dims;
};

#endif // #ifndef __Target_h_
