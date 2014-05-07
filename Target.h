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
	Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Room* rm, powerUpType ptype, int powerUpNumber);

	Ogre::Vector3 getDimensions() { return dims; }
	powerUpType getPowerUpType() { return powerType; }
	
	void setActive(bool v);
	bool checkActive() { return active; }

	void setReceiverID(int pID);
	int getReceiverID() { return receiverID; }
	int getIndex() { return index; }

protected:
	Ogre::Vector3 dims;
	Ogre::ParticleSystem* particleSystem;
	powerUpType powerType;


	bool active;
	int receiverID;
	int index;
};

#endif // #ifndef __Target_h_
