#ifndef __Solo_h_
#define __Solo_h_

#include "Network.h"
#include "MCP.h"
#include "Simulator.h"

class MCP;

class Solo
{
public:
	Solo(MCP* mcp);
	~Solo(void);
	
	void createScene(void);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	bool processUnbufferedInput(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);

	void updatePlayerVelocity(Player* p);
	void updateCamera(Ogre::Real elapseTime);
	bool mouseMoved(Ogre::Real relX, Ogre::Real relY);

	void createOverlays(PlayerCamera* playCam);
	void restrictPlayerMovement();

private:
	Ogre::SceneManager* sceneMgr;
	Network* gameNetwork;
	Simulator* gameSimulator;
	MCP* MasterControl;
	Music* gameMusic;
	
	
	bool gameStart;
	bool gameOver;
	bool gamePause;

	bool clientOrientationChange;

	/* Game Functionality Constants */
	Ogre::Real mMove; 			// Move Constant
	Ogre::Real mRotate;
	float sprintFactor;			// Sprint Constant

	/* Game Objects */
		/* Room */
	Room* gameRoom;
		/* Players */
	Player *player;
		/* Power Ups */
	Target *Power;
	Target *Speed;
	Target *JumpPower;
	Target *Restore;
		/* Disk */
	Disk* gameDisk;

	/* Players Camera */
	PlayerCamera* pCam;

	/* Client State Variables */
		/* Tiles List */
	vector<int> removedHTileList;
	vector<int> removedCTileList;
		/* Update Time Tracker */
	float timeSinceLastStateUpdate;
		/* Gap Check Time */ //Most likely needs to be in respective Player's class
	time_t gapStartTime;
	time_t gapEndTime;

	Ogre::Light* pointLight;

};

#endif // #ifndef __Solo_h_
