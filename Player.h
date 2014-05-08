#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"

enum stateIndex{LEFT, RIGHT, BACK, FORWARD, BOOST, JUMP, HOLDING, PLAYING, VIEWMODE};

class Disk;
class Room;
class GUI;

class Player : public GameObject
{
public:
	Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, int playerID,  Room* gameRoom);//Ogre::Vector3 roomDims);
	void setHolding(bool x); // Set whether the player is holding the disk
	void attachDisk(Disk* d); // Attach the disk to the player
	void setMovementRestriction(bool x);
	
	bool performJump(void);
	void increaseJump(void);
	void decreaseJump(void);
	void initializeStates(void);
	bool checkState(int index);
	void setState(int index, bool value);
	Ogre::Vector3 fillVelocityVector(Ogre::Real m, float sprintFactor);

	bool togglePlayerCanCatch(void);

	void animateCharacter(Ogre::String stateName);

	void setPlayerSpace(void);
	void changeGameRoom(Room* newGameRoom);
	void setPlayerStartingPosition(bool roomChangeFlag);
	
	int getPlayerID(void) { return playerID; }
	Ogre::Vector3 getPlayerDimensions(void) { return dimensions; }

	bool checkPlayerCanCatch(void) { return playerCanCatch; } 
	bool checkHolding(void) { return isHolding; } 
	Disk* getPlayerDisk(void) { return playerDisk; }

	Ogre::SceneNode* getPlayerSightNode(void) { return pSightNode; }
	Ogre::SceneNode* getPlayerCameraNode(void) { return pCamNode; }
	

	bool checkMovementRestriction(void) { return movementRestricted; }
	struct RoomSpace* getPlayerSpace() { return playerSpace; }
	Ogre::Vector3 getStartingPosition() { return startingPosition; }
	
	float updateBoost(bool pressed);
	
	Ogre::Entity* getMeshEntity() { return customPlayerEnt; }
	Ogre::AnimationState* getCustomAnimationState() { return customAnimationState; }
	void nullAnimationState() { customAnimationState = NULL; }

	int jumpTimer;
	bool jumpPowerActive;
    Ogre::ParticleSystem* tailParticle;	
	Ogre::SceneNode* particleNode;
	bool moving;
	bool catchAnimation;


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
	
	float remainingTime;
    clock_t oldTime;
    clock_t newTime;
};

#endif // #ifndef __Player_h_
