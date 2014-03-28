#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	connectionEstablished = false;
	init_serverSocket = NULL;
	UDP_gameSocket = NULL;
	TCP_gameSocket = NULL;
	TCP_portNum = 64669;
	maxSizeOfList = sizeof(MCP_Packet) * 10;
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

		if (clientReady <= 0)
		{
			// printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
			// perror("SDLNet_CheckSockets");
		}
		else if (clientReady)
		{
			// printf("ACTIVITY!!!!\n\n\n\n");
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
void Network::sendPacket(vector<MCP_Packet> packList)
{
	int numSent;
	int packListSize = packList.size() * sizeof(MCP_Packet);
	char buff[maxSizeOfList];

	memset(buff, 0, maxSizeOfList); // bigger size

	for (int i = 0; i < packList.size(); i++)
		memcpy(buff+(i*sizeof(MCP_Packet)), &packList[i], sizeof(MCP_Packet));

	numSent = SDLNet_TCP_Send(TCP_gameSocket, buff, packListSize);
	if (!numSent)
	{
		printf("*****Failed to send packets of size %d!\n\n", numSent);
	}
}
//-------------------------------------------------------------------------------------
vector<MCP_Packet> Network::receivePacket()
{
	// we are going to recv packetList, no iterate thru buffer and return a packList
	vector<MCP_Packet> packList;
	MCP_Packet pack;

	char buff[maxSizeOfList]; 

	memset(buff, 0, maxSizeOfList);

	int numRead;
	if ((numRead = SDLNet_TCP_Recv(TCP_gameSocket, buff, maxSizeOfList)) <= 0) 
	{
		pack.id = 'n';
		packList.push_back(pack);
		return packList;
	}

	for (int i = 0; i < maxSizeOfList; i += sizeof(MCP_Packet))
	{
		memcpy(&pack, buff+i, sizeof(MCP_Packet));

		if (pack.id == 'n')
			break;
		packList.push_back(pack);
	}

	return packList;
}
bool Network::checkConnection()
{
	return connectionEstablished;
}
bool Network::checkSockets()
{
	int clientReady = SDLNet_CheckSockets(i_set, 0);

	if (clientReady <= 0)
	{
		// printf("No Activity; Chiiiiiill!\n\n");
		return false;
	}
	else if (clientReady)
	{
		// printf("ACTIVITY!!!!\n\n");
		return true;
	}
}