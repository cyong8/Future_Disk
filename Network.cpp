#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	connectionEstablished = false;
	maxPacketSize = 256;
	init_serverSocket = NULL;
	UDP_gameSocket = NULL;
	TCP_gameSocket = NULL;
	TCP_portNum = 64669;
	i_set = SDLNet_AllocSocketSet(5);

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

		int numused = SDLNet_TCP_AddSocket(i_set, TCP_gameSocket);

		if (numused == -1 || numused == 0) 
		{
	    	printf("\n\n\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
	    	exit(2);
		}

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

	int numused = SDLNet_TCP_AddSocket(i_set, TCP_gameSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}

	connectionEstablished = true;
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(MCP_Packet pack)
{
	int numSent;
	char buff[sizeof(MCP_Packet)];
	memcpy(buff, &pack, sizeof(pack));

	numSent = SDLNet_TCP_Send(TCP_gameSocket, buff, sizeof(MCP_Packet));
	if (!numSent)
	{
		printf("*****Failed to send packet; Packet ID: %c\n\n", pack.id);
	}
}
//-------------------------------------------------------------------------------------
MCP_Packet Network::receivePacket()
{
	MCP_Packet pack;
	char buff[sizeof(MCP_Packet)];
	memset(buff, 0, sizeof(MCP_Packet));
	if (SDLNet_TCP_Recv(TCP_gameSocket, buff, sizeof(MCP_Packet)) <= 0)
	{
		pack.sequence = 'n';
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

	// printf("\t\tReceive - ID: %c\n", pack.id);
	// printf("\t\tReceive - X: %f\n", pack.x_coordinate);
	// printf("\t\tReceive - Y: %f\n", pack.y_coordinate);
	// printf("\t\tReceive - Z: %f\n\n\n", pack.z_coordinate);

	return pack;
}
bool Network::checkConnection()
{
	return connectionEstablished;
}
bool Network::checkSockets()
{
	int clientReady = SDLNet_CheckSockets(i_set, 0);

	if (clientReady == -1)
	{
		printf("No Activity; Chiiiiiill!\n\n");
		return false;
	}
	else if (clientReady)
	{
		printf("ACTIVITY!!!!\n\n");
		return true;
	}
}