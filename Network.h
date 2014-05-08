#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include "OgreQuaternion.h"

#define TCP_portNum 64669
#define MAX_NUMBER_OF_PLAYERS 4
#define MAX_SIZE_OF_BUFFER 4000

using namespace std;

enum identifier
{
	SERVER,
	CLIENT
};

enum gameStates
{
	START,
	QUIT,
	SOUND,
	PLAYERGAMESTATE,
	ENDROUND
};
enum keyID
{
	W,
	A,
	S,
	D,
	V,
	SPACE,
	SHIFT,
	MOUSECLICK,
	ESC,
	ENTER
};

enum packID
{
	C_PLAYER = 1,
	S_PLAYER = 2,
	DISK = 3,
	INPUT = 4,
	POWERUP = 5,
	TILE = 6,
	GAMESTATE = 7,
	EXPANSION = 8
};
struct C_PLAYER_packet
{
	char packetID;
	char playID;
	// orientation of respective player
	Ogre::Quaternion orientation;
};	
struct S_PLAYER_packet
{
	char packetID;
	char playID;
	// positions of each player
	float x;
	float y;
	float z;
	// orientation of each player
	Ogre::Quaternion orientation;
};
struct DISK_packet 
{
	char packetID;
	char diskID;
	char playID;
	char caught;
	// position of the disk
	float x;
	float y;
	float z;
	// orientation of the disk
	Ogre::Quaternion orientation;
};
struct INPUT_packet 
{
	char packetID;
	char playID;
	// keypressed
	char key;
};
struct POWERUP_packet 
{
	char packetID;
	char powerID;
	char receiverID;
	char index;
	int tileNumber;
	// positions of each power-up
	float x;
	float y;
	float z;
	// which power-ups are active
};
struct TILE_packet 
{
	char packetID;
	// tile removed/added
	char playID;
	char removed; // 0 = Restored, 1 = Removed
	int tileNumber;
};
struct GAMESTATE_packet 
{
	char packetID;
	char stateID;		// either player ID for resize or music ID
	char stateAttribute;	// type of state change
	// sound
	// GUI stuff- lose, win, gameStart
};
struct EXPANSION_packet  // deal with this later
{
	char packetID;
	// size and shape of room, which is dependent on the number of players
};

class Connection
{
public:
	TCPsocket sock;
	bool active;

	Connection()
	{
		sock = NULL;
		active = true;
	}
};

class Network
{
public:
	Network(int sc_identifier, char* hostIP);
	~Network();
	void startListening(void);
	int establishConnection(void);
	void acceptClient(void);
	int sendPacket(char* pack, int socketID);
	char* receivePacket(int socketID);
	bool checkSockets(int socketID);
	int getPacketSize(char type);
	void removeClient(int clientIndex);

private:
	SDLNet_SocketSet clientSet;
	TCPsocket initServerSocket;

	IPaddress serverIP;
	IPaddress* clientIP;
	char* serverIP_c;
	int UDP_portNum;
	int UDP_channel;
	identifier networkID;
	int numberOfConnections;
	int playerID;

	/* Client Specific Variables */
	TCPsocket clientSocket;

	/* Server Specific Variables */
	vector<Connection> connections;
};

#endif // #ifndef __Network_h_
