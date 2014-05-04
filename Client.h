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
	Room* twoPlayerGameRoom;
	Room* fourPlayerGameRoom;
	Room* activeRoom;
		/* Other Players */
	vector<Player*> playerList;
	int numPlayers;
	Player* opponent;
		/* Power Ups */
	vector<Target*> explosiveList;
	vector<Target*> speedList;
	vector<Target*> jumpList;
	vector<Target*> restoreList;
	vector<Target*> activePowerUps;
		/* Disk */
	Disk* gameDisk;

	Network* gameNetwork;
	Music* gameMusic;

	/* Game State Constants */
	bool clientOrientationChange;
	Ogre::Real mRotate;
	clock_t updateClock;
	bool gameStart;

	Ogre::Light* directLight;
	void updateScene(void);
	void updateCamera(void);
	void interpretServerPacket(char* packList);
	Ogre::Vector3 clientChangePosition(void);
	void switchRooms(int playersInRoom);
};

#endif // #ifndef __Client_h_