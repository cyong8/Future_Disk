#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"
#include "Player.h"

class Player;

class Disk : public GameObject
{
protected:
	bool offWallRotation;
	btVector3 thrownVelocity;
	Ogre::String initialPlayer;
	int playerIDOfHolder;
	Player* playerLastThrew;

public:
	Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer);
	void setThrownVelocity(btVector3 v);
	btVector3 getThrownVelocity(void);
	btVector3 getOldVelocity(void);
	void setOldVelocity(btVector3 v);
	Ogre::String checkInitialPlayer(void);
	void createNewParticleSystem(int index);
	bool activatePowerUp(Ogre::String name, Player* p);
	void resetPowerUp(void);
	void setHolder(int pID);
	int checkIDOfHolder(void);
	void setPlayerLastThrew(Player* p);
	Player* getPlayerLastThrew(void);
	
	int previousParticleSystem;
	bool needsOrientationUpdate;
	Ogre::ParticleSystem* tailParticle[5];
	Ogre::String powerUp;
	Ogre::SceneNode* particleNode;
};

#endif // #ifndef __Disk_h_
