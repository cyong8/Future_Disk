#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

class Disk;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setHolding(void);
	void attachDisk(Disk* d);
	bool checkHolding(void);
	bool checkIsInHand(void);
	Disk* getPlayerDisk(void);
	void setInHand(void);

	Ogre::SceneNode* getPlayerSightNode(void);
	Ogre::SceneNode* getPlayerCameraNode(void);
	Ogre::Vector3 getPlayerDimensions(void);


protected:
	bool isHolding;		
	bool isInHand;
	Disk* playerDisk;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* mSightNode; // The character is supposed to be looking here
};

#endif // #ifndef __Player_h_
