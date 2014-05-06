#ifndef __Server_h_
#define __Server_h_

#include "Network.h"
#include "MCP.h"
#include "Simulator.h"

class MCP;

class Server
{
public:
	Server(MCP* mcp);
	~Server(void);
	
	void createScene(void);
	bool frameRenderingQueued(Ogre::Real tSinceLastFrame);

private:
	Ogre::SceneManager* sSceneMgr;
	Network* gameNetwork;
	Simulator* gameSimulator;
	MCP* MasterControl;
	Music* gameMusic;
	int numberOfClients; // allows server to index into socketSet
						 // index in socketSet = playerID

/* Game Functionality Constants */
	/* Move Constants */
	Ogre::Real mMove; 			
	Ogre::Real mRotate;
	float sprintFactor;
	/* Psuedo Host Trigger */
	bool pseudoHostStartGame;
	bool gameRoomCreated;

/* Game Objects */
	/* Room */
	Room* twoPlayerGameRoom;
	Room* fourPlayerGameRoom;
	Room* activeRoom;
	/* Players */
	vector<Player*> playerList;
	/* Power Ups */
	vector<Target*> explosiveList;
	vector<Target*> speedList;
	vector<Target*> jumpList;
	vector<Target*> restoreList;
	vector<Target*> activePowerUps;
	vector<int> activePowerUpTypes;
	vector<Target*> removedPowerUps;
	/* Disk */
	Disk* gameDisk;

/* Client State Variables */
	/* Tiles List */
	vector<Tile*> removedTiles;
	/* Update Time Tracker */
	clock_t updateClock;
	bool forceUpdate;
	/* Gap Check Time */ //Most likely needs to be in respective Player's class
	time_t gapStartTime;
	time_t gapEndTime;

	Ogre::Light* directLight;
	void updateClientVelocity(Player* p);
	bool constructAndSendGameState(void);
	bool updateRemovedTiles(void);
	bool updatePowerUps(void);
	void restrictPlayerMovement(Player* p);
	bool interpretClientPacket(int playerID);
	void processClientInput(int playerIndex, char inputType);
	void removePlayer(int playerIndex);
	void switchRooms();
	void activatePowerUps();
	void restartRound(void);
};

#endif // #ifndef __Server_h_
