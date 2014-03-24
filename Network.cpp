#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	connectionEstablished = false;
	maxPacketSize = 256;
	init_serverSocket = NULL;
	UDP_gameSocket = NULL;
	TCP_gameSocket = NULL;
	TCP_portNum = 64669;
	i_set = SDLNet_AllocSocketSet(1);

	/*Initialize the network*/
	if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize net:%s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	if (sc_identifier == 0) // Set as server
	{
		server = 1;
		client = 0;
		initializeSockets();
	}
	if (sc_identifier == 1) // Set as client
	{
		server = 0;
		client = 1;
		serverIP_c = hostIP;
	}
}
//-------------------------------------------------------------------------------------
Network::~Network()
{
	SDLNet_TCP_Close(init_serverSocket);
	SDLNet_TCP_Close(TCP_gameSocket);
	SDLNet_UDP_Close(UDP_gameSocket);
	SDLNet_FreeSocketSet(i_set);
}
//-------------------------------------------------------------------------------------
void Network::initializeSockets()	// initialize the TCPsocket and Open the UDPsocket
{
	/* Initialize listening of the host for clients */
	if (SDLNet_ResolveHost(&serverIP, NULL, TCP_portNum) < 0) // NULL indicates listening
	{
	    printf("\n\n\nSDLNet_ResolveHost: %s\n", SDLNet_GetError());
	    exit(1);
	}
	//printf("\n\n\n\n\n\nServer IP: %x, %d\n\n\n\n", serverIP.host, serverIP.port);
	/* Open the TCP connection with the server IP found above */
	init_serverSocket = SDLNet_TCP_Open(&serverIP);	
	if(!init_serverSocket) 
	{
	    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
	    exit(2);
	}

	//---------------Opening UDP Socket -----------------------------------------
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

	/* Add the socket to the socket i_set so we can use CheckSockets() later */
	int numused = SDLNet_TCP_AddSocket(i_set, init_serverSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}
}
//-------------------------------------------------------------------------------------
bool Network::establishConnection()
{
	if (server)
	{
		std::ostringstream portData_ss;
		portData_ss << UDP_portNum;
		char portData[portData_ss.str().length()];
		strcpy(portData, portData_ss.str().c_str());

		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %s\n\n\n", portData);
				
		int clientReady = SDLNet_CheckSockets(i_set, 0);

		if (clientReady == -1)
		{
			printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			perror("SDLNet_CheckSockets");
		}
		else if (clientReady)
		{
			printf("ACTIVITY!!!!\n\n\n\n");
			acceptClient(portData);
		}
	}
	if (client)
	{
		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %s\n\n\n", serverIP_c);
		
		if (SDLNet_ResolveHost(&serverIP, serverIP_c, TCP_portNum) < 0) // Connects to the listening host
		{
	    	printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
	    	return false;
		}
		
		TCP_gameSocket = SDLNet_TCP_Open(&serverIP);

		if(!TCP_gameSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    return false;
		}
		/* Player has to wait for Server to send packet - Info of UDP */
		char portData[512];
		SDLNet_TCP_Recv(TCP_gameSocket, portData, 512);
		UDP_portNum = atoi(portData);

		printf("\n\n\n**********UDP Port Number that will facilitate game transactions: %d\n\n\n", UDP_portNum);

		//---------------Opening UDP Socket -----------------------------------------
		UDP_gameSocket = SDLNet_UDP_Open(0);				// Socket does not need to be binded to a channel; Open does this for UDP
		/************************************* HUGE CHANGE WITH UDP SOCKETS ON DIFFERENT PORTS *****************************************/

		if (UDP_gameSocket == NULL)							// Store the channel associated with the Socket; the player needs this
		{
		    printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		// UDP_channel = SDLNet_UDP_Bind(UDP_gameSocket, -1, &serverIP);
		// if(UDP_channel == -1) 
		// {
  //   		printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
  //   		exit(2);
		// }

		connectionEstablished = true;
	}

	return connectionEstablished;
}
//-------------------------------------------------------------------------------------
void Network::acceptClient(char *data)
{
	int length = strlen(data) + 1;

	TCP_gameSocket = SDLNet_TCP_Accept(init_serverSocket);

	if (TCP_gameSocket == NULL)
	{
		printf("TCP SOCKET NULL!\n\n\n");
		return;
	}
	SDLNet_TCP_Send(TCP_gameSocket, data, length);
	clientIP = SDLNet_TCP_GetPeerAddress(TCP_gameSocket);
	printf("Client connected - IP: %d\n\n", clientIP->host);
	/* UDP socket is open; now we have the IP of the client, so we can bind */
	// UDP_channel = SDLNet_UDP_Bind(UDP_gameSocket, -1, clientIP);
	// if(UDP_channel == -1) 
	// {
 //    	printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
 //    	exit(2);
	// }

	connectionEstablished = true;
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(MCP_Packet pack)
{
	int numSent;
	char buff[sizeof(struct MCP_Packet)];
	memcpy(buff, &pack, sizeof(pack));

	numSent = SDLNet_TCP_Send(TCP_gameSocket, buff, maxPacketSize);	
	if (!numSent)
	{
		printf("*****Failed to send packet; Packet ID: %c\n\n", pack.id);
	}
	else
		printf ("Size of packet sent: %d\n\n\n", numSent);

	/* UDP Packet/Sockets not working - Doing TCP for now */
	// UDPpacket *p;
	
	// if (!(p = SDLNet_AllocPacket(maxPacketSize)))
	// {
	// 	printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	// 	exit(2);
	// }

	// p->len = maxPacketSize;
	// p->data = (Uint8*)&pack;
	
	// if (client) /* Client sends data to server */
	// {
	// 	p->address.host = serverIP.host;	/* Set the destination host */
	// 	p->address.port = serverIP.port;	/* And destination port */
	// 	// p->channel = UDP_channel;			/* Use the specified channel for the server IP */

	// 	int checkSent;
	// 	checkSent = SDLNet_UDP_Send(UDP_gameSocket, p->channel, p);

	// 	if (!checkSent)
	// 		printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	// }
	// if (server) /* Server sends data to client */
	// {
	// 	p->address.host = clientIP->host;	/* Set the destination host */
	// 	p->address.port = clientIP->port;	/* And destination port */
	// 	// p->channel = UDP_channel;			/* Use the specified channel for the client IP */

	// 	int checkSent;
	// 	checkSent = SDLNet_UDP_Send(UDP_gameSocket, p->channel, p);

	// 	if (!checkSent)
	// 		printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	// }
	// SDLNet_FreePacket(p);
}
//-------------------------------------------------------------------------------------
MCP_Packet Network::receivePacket()
{
	MCP_Packet pack;
	char buff[maxPacketSize];
	if (SDLNet_TCP_Recv(TCP_gameSocket, buff, maxPacketSize) <= 0)
	{
		pack.id = 'n';
		return pack;
	}
	/* UDP Packet/Sockets not working - Doing TCP for now */
	// MCP_Packet* pack = NULL;
	// UDPpacket *p;
	
	// if (!(p = SDLNet_AllocPacket(maxPacketSize)))
	// {
	// 	fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
	// 	exit(EXIT_FAILURE);
	// }
	// if (SDLNet_UDP_Recv(UDP_gameSocket, p))	// ATM no patcket received
	// 	pack = (MCP_Packet*)p->data;

	// pack = (MCP_Packet*)&buf;
	memcpy(&pack, buff, sizeof(pack));

	printf("\t\tReceive - ID: %c\n", pack.id);
	printf("\t\tReceive - X: %f\n", pack.x_coordinate);
	printf("\t\tReceive - Y: %f\n", pack.y_coordinate);
	printf("\t\tReceive - Z: %f\n\n\n", pack.z_coordinate);

	return pack;
}
bool Network::checkConnection()
{
	return connectionEstablished;
}