#ifndef __Client_h_
#define __Client_h_

#include "Network.h"

class Player;
class Network;

class Client
{
public:
	Client(Player *p);
	~Client(void);
	
private:
	Player* clientPlayer;
};

#endif // #ifndef __Client_h_