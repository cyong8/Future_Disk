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
		SDLNet_TCP_Close(TCP_gameSocket);
		SDLNet_UDP_Close(UDP_gameSocket);
	}
	if (client == 1)	// clean up client side
	{
		SDLNet_TCP_Close(TCP_gameSocket);
		SDLNet_UDP_Close(UDP_gameSocket);
	}
}
//-------------------------------------------------------------------------------------
void Network::initializeConnection()
{
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
	UDP_gameSocket = NULL;
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
		for (int i = 49152; (UDP_gameSocket == NULL) && i < 65536; ++i) // Look thru Dynamic/Private ports and open socket at first available
		{
			/* Open UDP socket with the free port above */
			UDP_gameSocket = SDLNet_UDP_Open(i);		// Socket does not need to be binded to a channel; Open does this for UDP
			if (UDP_gameSocket != NULL)				// Store the channel associated with the Socket; the player needs this
			{
				UDP_portNum = i;
				break;
			}
		}
		/* Check if the socket was opened correctly */
		if(!UDP_gameSocket) 
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
		TCP_gameSocket = SDLNet_TCP_Open(&serverIP);

		if(!TCP_gameSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		/* Player has to wait for Server to send packet - Info of UDP */
		char portData[512];
		int result = 0;
		result = SDLNet_TCP_Recv(TCP_gameSocket, portData, 512);
		UDP_portNum = atoi(portData);

		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %d\n\n\n", UDP_portNum);

		UDP_gameSocket = SDLNet_UDP_Open(UDP_portNum);		// Socket does not need to be binded to a channel; Open does this for UDP
		if (UDP_gameSocket == NULL)				// Store the channel associated with the Socket; the player needs this
		{
		    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}

		SDLNet_TCP_Close(TCP_gameSocket);
	}
}
//-------------------------------------------------------------------------------------
int Network::getUDPPortNumber()
{
	return UDP_portNum;
}
//-------------------------------------------------------------------------------------
TCPsocket Network::getTCPSocket()
{
	return TCP_gameSocket;
}
//-------------------------------------------------------------------------------------
UDPsocket Network::getUDPSocket()
{
	return UDP_gameSocket;
}
//-------------------------------------------------------------------------------------
void Network::acceptClient(char *data, TCPsocket* sock)
{
	int length = strlen(data) + 1;
	TCP_gameSocket = SDLNet_TCP_Accept(*sock);

	if (TCP_gameSocket == NULL)
		return;
	SDLNet_TCP_Send(TCP_gameSocket, data, length);
	playerIP = SDLNet_TCP_GetPeerAddress(TCP_gameSocket);
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(MCP_Packet pack)
{
	int dataSize = sizeof(pack.sequence);
	dataSize += sizeof(pack.id);
	dataSize += sizeof(pack.X_coordinate);
	dataSize += sizeof(pack.Y_coordinate);
	dataSize += sizeof(pack.Z_coordinate);

	(char*)&pack;

	UDPpacket *p;
	p = SDLNet_AllocPacket(dataSize + sizeof(UDPpacket));

	p->len = dataSize + 1;

	SDLNet_UDP_Send(sd, -1, p);
	
	if (client)
	{
		p->address.host = serverIP.host;	/* Set the destination host */
		p->address.port = serverIP.port;	/* And destination port */

		//SDLNet_UDP_Send()
	}
	if (server)
	{
		p->address.host = serverIP.host;	/* Set the destination host */
		p->address.port = serverIP.port;	/* And destination port */
	}
}
//-------------------------------------------------------------------------------------
MCP_Packet Network::receivePacket()
{
	UDPpacket *p;
	MCP_Packet pack;

//	if (SDLNet_UDP_Recv(UDP_gameSocket, p))
	{
		
	}
	return pack;
}