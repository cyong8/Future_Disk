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
	bool frameRenderingQueued(const Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	bool processUnbufferedInput(const Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);

	void updatePlayerVelocity(Player* p);
	void updateCamera(Ogre::Real elapseTime);
	bool mouseMoved(Ogre::Real relX, Ogre::Real relY);

	void createOverlays(PlayerCamera* playCam);
	void restrictPlayerMovement();
	
	bool updateTimer(time_t currTime);
    void updatePauseTime(time_t currTime);
    
    void restartGame();
    
    void togglePause();
    
    void modifyScore(int increase);

private:
	Ogre::SceneManager* sceneMgr;
	Network* gameNetwork;
	Simulator* gameSimulator;
	MCP* MasterControl;
	Music* gameMusic;
	
	int score;

	bool gameStart;
	bool gameOver;
	bool gamePause;
	bool diskAdded;
	bool boostPenalty;

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
	vector<Target*> target_list;

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
	
	int initMinutes;
	time_t initTime;
    double pTimePassed;
    time_t pauseTime;

	Ogre::Light* pointLight;

};

#endif // #ifndef __Solo_h_
