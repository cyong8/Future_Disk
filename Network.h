#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>

#define TCP_portNum 64669

struct MCP_Packet
{
	char* sequence;
	char* id;
	char* X_coordinate;
	char* Y_coordinate;
	char* Z_coordinate;
};

class Network
{
public:
	Network(int sc_identifier, char* hostIP);
	~Network();
	void initializeConnection(void);
	bool waitForPacket(void);
	int getUDPPortNumber(void);
	TCPsocket getTCPSocket(void);
	UDPsocket getUDPSocket(void);
	void acceptClient(char* data, TCPsocket* sock);
	void sendPacket(MCP_Packet pack);
	MCP_Packet receivePacket(void);

private:
	TCPsocket TCP_gameSocket;
	UDPsocket UDP_gameSocket;
	IPaddress serverIP;
	IPaddress* playerIP;
	char* serverIP_c;
	int UDP_portNum;
	int server; 
	int client;
};

#endif // #ifndef __Network_h_
