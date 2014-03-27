#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

enum stateIndex{Left, Right, Back, Forward, Jump, Boost};

class Disk;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition, Ogre::String side);

	void setHolding(void); // Set whether the player is holding the disk
	bool checkHolding(void); // Check if the player is holding the disk
	void throwDisk(void); // Throw the disk in a particular direction
	void attachDisk(Disk* d); // Attach the disk to the player

	Disk* getPlayerDisk(void); // Get the disk that the player is holding
	Ogre::SceneNode* getPlayerSightNode(void); // Get the scene node where the player is looking
	Ogre::SceneNode* getPlayerCameraNode(void); // Get the camera node attached to the player
	Ogre::Vector3 getPlayerDimensions(void); // Get the dimensions of the player
	
	void setGroundY(float y);
	Ogre::Real getGroundY(void);
	bool performJump(void);
	Ogre::String checkPlayerSide(void);
	bool groundConstantSet;
	Ogre::Real jumpFactor;
	void increaseJump(void);
	void decreaseJump(void);
	void initializeStates(void);
	bool checkState(int index);
	void toggleState(int index);
	void resetPlayerState(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard);
	Ogre::Vector3 fillClientVelocityVector(Ogre::Real m, float sprintFactor);
	int jumpTimer;
	bool jumpPowerActive;
    Ogre::ParticleSystem* tailParticle;	
	Ogre::SceneNode* particleNode;

protected:
	vector<bool> states;
	bool isHolding;		
	Disk* playerDisk;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* pSightNode; // The character is supposed to be looking here
	Ogre::Real groundY;
	Ogre::Real prevGroundY;
	Ogre::String playerSide;
};

#endif // #ifndef __Player_h_
