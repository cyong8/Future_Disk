#ifndef __Network_h_
#define __Network_h_

#include "SDL_net.h"

enum enumGameObject{Player, Disk, Wall, Target};
enum enumKeyboard{W, A, S, D};


class Network
{
public:
	Network();
	~Network();

	UDPsocket serverSocket;
	UDPsocket playerSocket;


};

struct Packet
{
	u_int8_t id;
	u_int32_t X_coordinate;
	u_int32_t Y_coordinate;
	u_int32_t Z_coordinate;
};

#endif // #ifndef __Network_h_
