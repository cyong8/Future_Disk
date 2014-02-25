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
	void setInHand(void);
	void throwDisk(void);

	Ogre::SceneNode* getPlayerSightNode(void);
	Ogre::SceneNode* getPlayerCameraNode(void);
	Ogre::Vector3 getPlayerDimensions(void);
	Disk* getPlayerDisk(void);


protected:
	bool isHolding;		
	bool isInHand;
	Disk* playerDisk;
	// Ogre::Vector3 crossHairDimensions;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* pSightNode; // The character is supposed to be looking here
	// Ogre::SceneNode* crossHairNode; // The helper SceneNode  
};

#endif // #ifndef __Player_h_
