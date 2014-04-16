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
	bool frameRenderingQueued(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	bool processUnbufferedInput(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse);
	bool mouseMoved(Ogre::Real revX, Ogre::Real revY);
	Player* getPlayer(void);

private:
	Ogre::SceneManager* cSceneMgr;

	/* Client's controlled player */
	Player* clientPlayer;
	int playerID;

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

	Ogre::Light* pointLight;
	void updateScene(void);
	void updateCamera(Ogre::Real elapseTime);
	void interpretServerPacket(char* packList);
};

#endif // #ifndef __Client_h_