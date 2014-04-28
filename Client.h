#ifndef __Client_h_
#define __Client_h_

#include "Network.h"
#include "MCP.h"

class Player;
class Network;

class Client
{
public:
	Client(char* IP, Ogre::SceneManager* mgr);
	~Client();

	void createScene(void);
	bool frameRenderingQueued(Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	void processUnbufferedInput(OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	bool mouseMoved(Ogre::Real relX, Ogre::Real relY);
	Player* getPlayer(void);
	void createOverlays(PlayerCamera* playCam);

private:
	Ogre::SceneManager* cSceneMgr;

	/* Client's controlled player */
	Player* clientPlayer;
	int playerID;
	Ogre::Vector3 previousPosition;

	/* Client's Player Camera */
	PlayerCamera* pCam;

	/* Other Objects in the Scene */
		/* Room */
	Room* gameRoom;
		/* Other Players */
	vector<Player*> playerList;
	int numPlayers;
	Player* opponent;
		/* Power Ups */
	Target *Power;
	Target *Speed;
	Target *JumpPower;
	Target *Restore;
		/* Disk */
	Disk* gameDisk;

	Network* gameNetwork;
	Music* gameMusic;

	/* Game State Constants */
	bool clientOrientationChange;
	Ogre::Real mRotate;
	bool gameStart;
	clock_t updateClock;

	Ogre::Light* directLight;
	void updateScene(void);
	void updateCamera(void);
	void interpretServerPacket(char* packList);
	Ogre::Vector3 clientChangePosition(void);
};

#endif // #ifndef __Client_h_