#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

enum stateIndex{LEFT, RIGHT, BACK, FORWARD, BOOST, VIEWMODE};

class Disk;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, int playerID, Ogre::Vector3 roomDims);
	// NOTE: roomDims is Ogre::Vector3(room->width, room->height), numberOfPlayers)
	void setHolding(bool x); // Set whether the player is holding the disk
	bool checkHolding(void); // Check if the player is holding the disk
	void throwDisk(void); // Throw the disk in a particular direction
	void attachDisk(Disk* d); // Attach the disk to the player
	Ogre::String getPlayerSide(void);
	void setMovementRestriction(bool x);
	bool checkMovementRestriction(void);

	Disk* getPlayerDisk(void); // Get the disk that the player is holding
	Ogre::SceneNode* getPlayerSightNode(void); // Get the scene node where the player is looking
	Ogre::SceneNode* getPlayerCameraNode(void); // Get the camera node attached to the player
	Ogre::Vector3 getPlayerDimensions(void); // Get the dimensions of the player
	
	int getPlayerID(void);
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
	void setState(int index, bool value);
	Ogre::Vector3 fillVelocityVector(Ogre::Real m, float sprintFactor);

	bool checkPlayerCanCatch(void);
	bool togglePlayerCanCatch(void);
	
	Ogre::Vector3 getStartingPosition() { return startingPosition; }

	int jumpTimer;
	bool jumpPowerActive;
    Ogre::ParticleSystem* tailParticle;	
	Ogre::SceneNode* particleNode;
	Ogre::Entity* ent;
	Ogre::AnimationState *animationState;


protected:
	vector<bool> states;
	Disk* playerDisk;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* pSightNode; // The character is supposed to be looking here
	Ogre::Real groundY;
	Ogre::Real prevGroundY;
	Ogre::String playerSide;
	bool isHolding;
	bool movementRestricted;	
	int playerID;	
	bool playerCanCatch;
	Ogre::Vector3 roomDimensions;
	Ogre::Vector3 startingPosition;
};

#endif // #ifndef __Player_h_
