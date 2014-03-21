#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	if (sc_identifier == 0) // Set as server
	{
		server = 1;
		client = 0;
	}
	if (sc_identifier == 1) // Set as client
	{
		server = 0;
		client = 1;
		serverIP_c = hostIP;
	}

	/*Initialize the network*/
	if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize net:%s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	/* Initialize TCP connection between Client and Server and pass UDP port number */
	initializeConnection();
}
//-------------------------------------------------------------------------------------
Network::~Network()
{
	if (server == 1) // clean up server side
	{
		SDLNet_TCP_Close(TCP_serverSocket);
		SDLNet_UDP_Close(serverSocket);
	}
	if (client == 1)	// clean up client side
	{
		SDLNet_TCP_Close(TCP_playerSocket);
		SDLNet_UDP_Close(playerSocket);
	}
}
//-------------------------------------------------------------------------------------
void Network::initializeConnection()
{
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
	serverSocket = NULL;
	if (server)
	{
		TCPsocket init_serverSocket;

		/* Initialize listening of the host for clients */
		if (SDLNet_ResolveHost(&serverIP, NULL, TCP_portNum) == -1) // NULL indicates listening
		{
		    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		    exit(1);
		}
		/* Open the TCP connection with the server IP found above */
		init_serverSocket = SDLNet_TCP_Open(&serverIP);	
		if(!init_serverSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		/* Find the UDP Port number that will be used for game state transactions */
		for (int i = 49152; (serverSocket == NULL) && i < 65536; ++i) // Look thru Dynamic/Private ports and open socket at first available
		{
			/* Open UDP socket with the free port above */
			serverSocket = SDLNet_UDP_Open(i);		// Socket does not need to be binded to a channel; Open does this for UDP
			if (serverSocket != NULL)				// Store the channel associated with the Socket; the player needs this
			{
				UDP_portNum = i;
				break;
			}
		}
		/* Check if the socket was opened correctly */
		if(!serverSocket) 
		{
		    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		std::ostringstream portData_ss;
		portData_ss << UDP_portNum;
		char portData[portData_ss.str().length()];
		strcpy(portData, portData_ss.str().c_str());

		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %s\n\n\n", portData);
				
		SDLNet_TCP_AddSocket(set, init_serverSocket);

		int clientReady = SDLNet_CheckSockets(set, 60000);

		if (clientReady == -1)
		{
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
    		//most of the time this is a system error, where perror might help you.
    		perror("SDLNet_CheckSockets");
		}
		else
		{
			acceptClient(portData, &init_serverSocket);
		}	
		SDLNet_TCP_Close(init_serverSocket);
	}
	if (client)
	{
		int wait;
		// while (wait)
		// {
			if ((wait = SDLNet_ResolveHost(&serverIP, serverIP_c, TCP_portNum)) == -1) // Connects to the listening host
			{
		    	printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		    	exit(1);
			}
		// }
		TCP_playerSocket = SDLNet_TCP_Open(&serverIP);

		if(!TCP_playerSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		/* Player has to wait for Server to send packet - Info of UDP */
		char portData[512];
		int result = 0;
		result = SDLNet_TCP_Recv(TCP_playerSocket, portData, 512);
		UDP_portNum = atoi(portData);

		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %s\n\n\n", portData);
		SDLNet_TCP_Close(TCP_playerSocket);
	}
}
//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
void Network::readPacket(UDPpacket* p)
{
	/* Packet Notes:
		- Apparently packets are not dropped unless they are larger than the buffer, so since we control
		packets, we just have to make sure that the buffer is greater than the largest packet we will send
		- Must close a packet after you are done using it; SDLNet_FreePacket(p);
	*/
}
//-------------------------------------------------------------------------------------
int Network::getUDPPortNumber()
{
	return UDP_portNum;
}
//-------------------------------------------------------------------------------------
UDPsocket Network::getServerSocket()
{
	return serverSocket;
}
//-------------------------------------------------------------------------------------
UDPsocket Network::getPlayerSocket()
{
	return playerSocket;
}
void Network::acceptClient(char *data, TCPsocket* sock)
{
	int length = strlen(data) + 1;
	TCP_serverSocket = SDLNet_TCP_Accept(*sock);
	if (TCP_serverSocket == NULL)
		return;
	SDLNet_TCP_Send(TCP_serverSocket, &data, length);
	playerIP = SDLNet_TCP_GetPeerAddress(TCP_serverSocket);
}