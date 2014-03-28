#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include "OgreQuaternion.h"

using namespace std;

struct MCP_Packet
{
	char id;
	float x_coordinate;
	float y_coordinate;
	float z_coordinate;
	Ogre::Quaternion orientationQ;
};						// Max Total = 16 Bytes - Must alloc at least 16 Bytes for buffer

class Network
{
public:
	Network(int sc_identifier, char* hostIP);
	~Network();
	void initializeSockets(void);
	bool establishConnection(void);
	bool waitForPacket(void);
	void acceptClient(char* data);
	void sendPacket(vector<MCP_Packet>);
	vector<MCP_Packet> receivePacket(void);
	bool checkConnection(void);
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
	Uint16 TCP_portNum;
	int server; 
	int client;
	int maxSizeOfList;
	bool connectionEstablished;
};

#endif // #ifndef __Network_h_
