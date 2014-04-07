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

using namespace std;

enum identifier
{
	SERVER,
	CLIENT
};

struct MCP_Packet
{
	char id;
	float x_coordinate;
	float y_coordinate;
	float z_coordinate;
	Ogre::Quaternion orientationQ;
	int tileIndex;
};						// Max Total = 16 Bytes - Must alloc at least 16 Bytes for buffer

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
	SDLNet_SocketSet i_set;
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
