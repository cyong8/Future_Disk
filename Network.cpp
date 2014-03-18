#include "Network.h"

Network::Network()
{
	// readPacket();
	if(SDL_Init(0) < 0)
	{
		fprintf(srderr, "Couldn't initialize SDL: %s/n", SDL_GetError());
		exit(1);
	}
	/*Initialize the network*/
	if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize net:%s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	serverSocket = SDLNet_UDP_Open(6666);

}

Socket* Network::CreateSocket()
{
	SDLNet_ResolveHost(&serverIP, NULL, CHAT_PORT);
	printf("ServerID: %x, %d\n", serverIP.host, serverIP.port);
	servsock = SDLNet_TCP
}

void Network::readPacket(Packet* p)
{
	u_int8_t identifier = p.id;

	switch (p.id)
	{
		case enumGameObject.Player:
			parsePlayerPacket();
			break;
		case enumGameObject.Disk:
	}
}