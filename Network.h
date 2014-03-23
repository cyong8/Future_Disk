#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

struct MCP_Packet
{
	char* sequence;		// Max = 2 Bytes - 16 Bits
	char id;			// Max = 2 Bytes - 16 Bits
	float X_coordinate;	// Max = 4 Bytes - 32 Bits
	float Y_coordinate;
	float Z_coordinate;
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
	void sendPacket(MCP_Packet pack);
	MCP_Packet* receivePacket(void);
	bool checkConnection(void);

private:
	SDLNet_SocketSet i_set;
	TCPsocket init_serverSocket;
	TCPsocket TCP_gameSocket;
	UDPsocket UDP_gameSocket;
	IPaddress serverIP;
	IPaddress* playerIP;
	char* serverIP_c;
	int UDP_portNum;
	Uint16 TCP_portNum;
	int server; 
	int client;
	int maxPacketSize;
	bool connectionEstablished;
};

#endif // #ifndef __Network_h_
