#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"	
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

#define TCP_portNum 64669

enum enumGameObject{};
enum enumKeyboard{};


class Network
{
public:
	Network(int sc_identifier);
	~Network();
	void initializeConnection(void);
	bool waitForPacket(void);
	void readPacket(UDPpacket* p);
	int getUDPPortNumber(void);
	UDPsocket getServerSocket(void);
	UDPsocket getPlayerSocket(void);

private:
	TCPsocket init_newServerSocket;
	TCPsocket init_playerSocket;
	UDPsocket serverSocket;
	UDPsocket playerSocket;
	IPaddress* serverIP;
	IPaddress* playerIP;
	int UDP_portNum;
	int server; 
	int client;
};

struct Packet
{
	u_int8_t id;
	u_int32_t X_coordinate;
	u_int32_t Y_coordinate;
	u_int32_t Z_coordinate;
};

#endif // #ifndef __Network_h_