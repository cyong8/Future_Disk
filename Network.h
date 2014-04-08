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

using namespace std;

enum identifier
{
	SERVER,
	CLIENT
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
	PLAYER,
	INPUT,
	DISK,
	POWERUP,
	TILE,
	GAMESTATE,
	EXPANSION
};

enum powerUpID
{
	nPOWER,
	nSPEED,
	nJUMP,
	nRESTORE
};

struct MCP_Packet
{
	char packetID;
};			
struct PLAYER_packet: MCP_Packet
{
	char playID;
	// positions of each player
	float x;
	float y;
	float z;
	// orientation of each player
	Ogre::Quaternion orientation;
};
struct INPUT_packet : MCP_Packet
{
	char playID;
	// keypressed
	keyID key;
};
struct DISK_packet : MCP_Packet
{
	char diskID;
	// position of the disk
	float x;
	float y;
	float z;
	// orientation of the disk
	Ogre::Quaternion orientation;
};
struct POWERUP_packet : MCP_Packet
{
	powerUpID powerID;
	// positions of each power-up
	float x;
	float y;
	float z;
	// which power-ups are active
};
struct TILE_packet : MCP_Packet
{
	// tile removed/added
	char playID;
	bool removed;
	short tileNumber;
};
struct GAMESTATE_packet : MCP_Packet
{
	int stateChange;	// type of state change
	// Activate respective power up 
	// sound
	// GUI stuff- lose, win, gameStart
};
struct EXPANSION_packet : MCP_Packet // deal with this later
{
	// size and shape of room, which is dependent on the number of players
};

class Network
{
public:
	Network(int sc_identifier, char* hostIP);
	~Network();
	void startListening(void);
	int establishConnection(void);
	void acceptClient(void);
	void sendPacket(vector<MCP_Packet>, int socketID);
	vector<MCP_Packet> receivePacket(int socketID);
	bool checkSockets(void);

private:
	SDLNet_SocketSet iSet;
	SDLNet_SocketSet clientSet;
	TCPsocket init_serverSocket;
	TCPsocket TCP_gameSocket;
	UDPsocket UDP_gameSocket;
	IPaddress serverIP;
	IPaddress* clientIP;
	char* serverIP_c;
	int UDP_portNum;
	int UDP_channel;
	identifier networkID;
	int maxSizeOfList;
	int numberOfConnections;
	bool connectionEstablished;
	int playerID;

	/* Client Specific Variables */
	TCPsocket clientSocket;

	/* Server Specific Variables */
	vector<TCPsocket> clientSocketList;

};

#endif // #ifndef __Network_h_
