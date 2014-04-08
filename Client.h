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
	Player* getPlayer(void);

private:
	Ogre::SceneManager* cSceneMgr;

	/* Client's controlled player */
	Player* clientPlayer;
	int playerID;

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

	/* Players Camera */
	PlayerCamera* pCam;

	Network* gameNetwork;
	Music* gameMusic;

	/* Game State Constants */
	bool clientOrientationChange;
	Ogre::Real mRotate;
	bool gameStart;

	Ogre::Light* pointLight;
	bool resetMovementState(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, vector<MCP_Packet> &packList);
	void updateScene(void);
	void updateCamera(Ogre::Real elapseTime);
	void interpretServerPacket(MCP_Packet packList);
	bool mouseMoved(const OIS::MouseEvent &evt);
};

#endif // #ifndef __Client_h_