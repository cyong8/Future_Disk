#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Player;

class Disk : public GameObject
{
protected:
	bool offWallRotation;
	btVector3 thrownVelocity;
	btVector3 oldVelocity;
	Ogre::String initialPlayer;

public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer);
	void setThrownVelocity(btVector3 v);
	btVector3 getThrownVelocity(void);
	btVector3 getOldVelocity(void);
	void setOldVelocity(btVector3 v);
	Ogre::Vector3 diskDirection;
	Ogre::ParticleSystem* tailParticle[9];
	Ogre::SceneNode* particleNode;
	bool needsOrientationUpdate;
	int previousParticleSystem;
	Ogre::String checkInitialPlayer(void);
};

#endif // #ifndef __Disk_h_
