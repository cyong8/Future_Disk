#ifndef __Server_h_
#define __Server_h_

#include "Network.h"
#include "MCP.h"
#include "Simulator.h"

class Server
{
public:
	Server(MCP* mcp);
	~Server(void);
private:
	Ogre::SceneManager* sSceneMgr;
	Network* gameNetwork;
	Simulator* gameSimulator;
	MCP* MasterControl;
	Music* gameMusic;

	/* Game Objects */
		/* Room */
	Room* gameRoom;
		/* Players */
	Player* p1;
	Player* p2;
		/* Power Ups */
	Target *Power;
	Target *Speed;
	Target *JumpPower;
	Target *Restore;
		/* Disk */
	Disk* gameDisk;

	Ogre::Light* pointLight;

	void createScene(void);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool processBufferedInput(const Ogre::FrameEvent& evt);
};

#endif // #ifndef __Server_h_
