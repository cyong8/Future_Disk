#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"
#include "Target.h"

class Player;

class Disk : public GameObject
{
protected:
	bool offWallRotation;
	btVector3 thrownVelocity;
	int playerIDOfHolder;
	Player* playerLastThrew;
	powerUpType powerUp;
	int diskNumber;

public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer, int num);
	void setThrownVelocity(btVector3 v);
	btVector3 getThrownVelocity(void);
	btVector3 getOldVelocity(void);
	void setOldVelocity(btVector3 v);
	void createNewParticleSystem(int index);
	bool activatePowerUp(powerUpType name, Player* p);
	void resetPowerUp(void);
	void setHolder(int pID);
	int checkIDOfHolder(void);
	void setPlayerLastThrew(Player* p);
	Player* getPlayerLastThrew(void);
	powerUpType checkActivePowerUp(void) { return powerUp; }
	int getDiskID(void) { return diskNumber; }

	int previousParticleSystem;
	bool needsOrientationUpdate;
	Ogre::ParticleSystem* tailParticle[5];
	Ogre::SceneNode* particleNode;


	Ogre::Entity* diskEnt;
	Ogre::AnimationState* diskAnimationState;
};

#endif // #ifndef __Disk_h_
