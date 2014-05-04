#ifndef __Target_h_
#define __Target_h_

#include "GameObject.h"

#define NUM_OF_TARGETS  7
#define NUM_OF_POWERUP_TYPES 4

enum powerUpType
{
	EXPLOSIVE, 
	SPEED, 
	JUMPBOOST, 
	RESTORE,
	TARGET
};

class Room;

class Target : public GameObject
{
public:
	Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Room* rm, powerUpType ptype);
	void targetHit();
	void resetHit();
	bool isHit(); 
	Ogre::Vector3 getDimensions() { return dims; }
	powerUpType getPowerUpType() { return powerType; }

protected:
	Ogre::Vector3 dims;
	powerUpType powerType;
};

#endif // #ifndef __Target_h_
