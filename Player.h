#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

class Disk;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);

	void setHolding(void); // Set whether the player is holding the disk
	void attachDisk(Disk* d); // Attach the disk to the player
	bool checkHolding(void); // Check if the player is holding the disk
	void throwDisk(void); // Throw the disk in a particular direction

	Ogre::SceneNode* getPlayerSightNode(void); // Get the scene node where the player is looking
	Ogre::SceneNode* getPlayerCameraNode(void); // Get the camera node attached to the player
	Ogre::Vector3 getPlayerDimensions(void); // Get the dimensions of the player
	Disk* getPlayerDisk(void); // Get the disk that the player is holding

protected:
	bool isHolding;		
	Disk* playerDisk;
	// Ogre::Vector3 crossHairDimensions;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* pSightNode; // The character is supposed to be looking here
};

#endif // #ifndef __Player_h_
