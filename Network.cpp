#include "Network.h"

Network::Network()
{
	/*Initialize the network*/
	if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize net:%s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}
	/* Open the server socket */ 
	for (int i = 1024; (serverSocket == NULL) && i < 65536; ++i) // Look thru Dynamic/Private ports and open socket at first available
	{
		serverSocket = SDLNet_UDP_Open(i);		// Socket does not need to be binded to a channel; Open does this for UDP
		if (serverSocket != NULL)				// Store the channel associated with the Socket; the player needs this
			serverChannel = i;
	}
	/* Check if the socket was opened correctly */
	if(!serverSocket) 
	{
	    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	    exit(2);
	}

	/* Open the player socket */ 
	for (int i = 1024; (playerSocket == NULL) && i < 65536; ++i)
	{
		playerSocket = SDLNet_UDP_Open(i);
		if (playerSocket != NULL)
			playerChannel = i;
	}
	/* Check if the socket was opened correctly */
	if(!playerSocket) 
	{
	    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	    exit(2);
	}
}
bool Network::waitForPacket()
{
	UDPpacket* p;
	if (!(p = SDLNet_AllocPacket(1024)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	}
	while (1)
	{
		if (SDLNet_UDP_Recv(serverSocket, p))
		{
			printf("UDP Packet incoming\n");
			printf("\tChan:    %d\n", p->channel);
			printf("\tData:    %s\n", (char *)p->data);
			printf("\tLen:     %d\n", p->len);
			printf("\tMaxlen:  %d\n", p->maxlen);
			printf("\tStatus:  %d\n", p->status);
			printf("\tAddress: %x %x\n", p->address.host, p->address.port);
		}		
	}
}
void Network::readPacket(UDPpacket* p)
{
	/* Packet Notes:
		- Apparently packets are not dropped unless they are larger than the buffer, so since we control
		packets, we just have to make sure that the buffer is greater than the largest packet we will send
		- Must close a packet after you are done using it; SDLNet_FreePacket(p);
	*/
	
}
int Network::getServerChannel()
{
	return serverChannel;
}
int Network::getPlayerChannel()
{
	return playerChannel;
}
UDPsocket Network::getServerSocket()
{
	return serverSocket;
}
UDPsocket Network::getPlayerSocket()
{
	return playerSocket;
}