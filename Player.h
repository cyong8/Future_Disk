#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

enum stateIndex{LEFT, RIGHT, BACK, FORWARD, BOOST, JUMP, HOLDING, PLAYING, VIEWMODE};

class Disk;
class Room;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, int playerID,  Room* gameRoom);//Ogre::Vector3 roomDims);
	void setHolding(bool x); // Set whether the player is holding the disk
	bool checkHolding(void); // Check if the player is holding the disk
	void attachDisk(Disk* d); // Attach the disk to the player
	Ogre::String getPlayerSide(void);
	void setMovementRestriction(bool x);
	bool checkMovementRestriction(void);

	Disk* getPlayerDisk(void); // Get the disk that the player is holding
	Ogre::SceneNode* getPlayerSightNode(void); // Get the scene node where the player is looking
	Ogre::SceneNode* getPlayerCameraNode(void); // Get the camera node attached to the player
	Ogre::Vector3 getPlayerDimensions(void); // Get the dimensions of the player
	
	int getPlayerID(void);
	bool performJump(void);
	void increaseJump(void);
	void decreaseJump(void);
	void initializeStates(void);
	bool checkState(int index);
	void setState(int index, bool value);
	Ogre::Vector3 fillVelocityVector(Ogre::Real m, float sprintFactor);

	bool checkPlayerCanCatch(void);
	bool togglePlayerCanCatch(void);
	
	Ogre::Vector3 getStartingPosition() { return startingPosition; }
	Ogre::Entity* getMeshEntity() { return customPlayerEnt; }
	Ogre::AnimationState* getCustomAnimationState() { return customAnimationState; }
	void animateCharacter(Ogre::String stateName);

	void setPlayerSpace();
	struct RoomSpace* getPlayerSpace() { return playerSpace; };

	int jumpTimer;
	bool jumpPowerActive;
    Ogre::ParticleSystem* tailParticle;	
	Ogre::SceneNode* particleNode;


protected:
	/* Custom Mesh Attributes */
	Ogre::Entity* customPlayerEnt;
	Ogre::AnimationState* customAnimationState;

	/* Player Specific Attributes */ 
	vector<bool> states;
	Ogre::Vector3 dimensions;
	Ogre::SceneNode* pCamNode;   // The camera is supposed to be here
	Ogre::SceneNode* pSightNode; // The character is supposed to be looking here
	Ogre::String playerSide;
	bool isHolding;
	bool movementRestricted;	
	int playerID;	
	bool playerCanCatch;
	Ogre::Vector3 startingPosition;
	
	/* Game Attributes */ 
	Ogre::Real jumpFactor;
	Disk* playerDisk;
	Room* playerRoom;
	struct RoomSpace* playerSpace;
};

#endif // #ifndef __Player_h_
