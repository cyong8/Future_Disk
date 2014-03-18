#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"
#include <stdio.h>
#include <stdlib.h>

enum enumGameObject{};
enum enumKeyboard{};


class Network
{
public:
	Network();
	~Network();
	bool waitForPacket(void);
	void readPacket(UDPpacket* p);
	int getServerChannel(void);
	int getPlayerChannel(void);
	UDPsocket getServerSocket(void);
	UDPsocket getPlayerSocket(void);

private:
	UDPsocket serverSocket;
	UDPsocket playerSocket;
	int serverChannel;
	int playerChannel;
};

struct Packet
{
	u_int8_t id;
	u_int32_t X_coordinate;
	u_int32_t Y_coordinate;
	u_int32_t Z_coordinate;
};

#endif // #ifndef __Network_h_
