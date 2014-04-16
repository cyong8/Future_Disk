#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	numberOfConnections = 0;
	connectionEstablished = false;
	init_serverSocket = NULL;
	clientSocket = NULL;
	clientSocketList = vector<TCPsocket>(MAX_NUMBER_OF_PLAYERS, NULL);
	UDP_gameSocket = NULL;
	TCP_gameSocket = NULL;
	iSet = SDLNet_AllocSocketSet(1);
	clientSet = SDLNet_AllocSocketSet(4);

	/*Initialize the network*/
	if(SDLNet_Init() < 0)
	{
		fprintf(stderr, "Couldn't initialize net:%s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	if (sc_identifier == SERVER) // Set as server
	{
		networkID = SERVER;
		startListening();
	}
	if (sc_identifier == CLIENT) // Set as client
	{
		networkID = CLIENT;
		serverIP_c = hostIP;
	}
}
//-------------------------------------------------------------------------------------
Network::~Network()
{
	SDLNet_TCP_Close(init_serverSocket);
	SDLNet_TCP_Close(TCP_gameSocket);
	SDLNet_TCP_Close(clientSocket);
	SDLNet_UDP_Close(UDP_gameSocket);
	SDLNet_FreeSocketSet(clientSet);
	SDLNet_FreeSocketSet(iSet);
}
//-------------------------------------------------------------------------------------
void Network::startListening()
{
	/* Initialize listening of the host for clients */
	if (SDLNet_ResolveHost(&serverIP, NULL, TCP_portNum) < 0) // NULL indicates listening
	{
	    printf("\n\n\nSDLNet_ResolveHost: %s\n", SDLNet_GetError());
	    exit(1);
	}
	/* Open the TCP connection with the server IP found above */
	init_serverSocket = SDLNet_TCP_Open(&serverIP);	

	if(!init_serverSocket) 
	{
	    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
	    exit(2);
	}

	/* Add the socket to the socket clientSet so we can use CheckSockets() later */
	int numused = SDLNet_TCP_AddSocket(iSet, init_serverSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}
}
//-------------------------------------------------------------------------------------
int Network::establishConnection()
{
	if (networkID == SERVER) 
	{
		// std::ostringstream portData_ss; // UDP_port num - not currently used
		// portData_ss << UDP_portNum;
		// char portData[portData_ss.str().length()];
		// strcpy(portData, portData_ss.str().c_str());

		if (SDLNet_SocketReady(init_serverSocket))
		{
			// printf("ACTIVITY!!!!\n\n\n\n");
			acceptClient();
			return 1;
		}
	}
	if (networkID == CLIENT)
	{	
		char idBuff[2];

		if (SDLNet_ResolveHost(&serverIP, serverIP_c, TCP_portNum) < 0) // Connects to the listening host
		{
	    	printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
	    	exit(2);
		}

		clientSocket = SDLNet_TCP_Open(&serverIP);

		if(!clientSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    exit(2);
		}
		/* Player has to wait for Server to send packet - Info of UDP  NOT IMPLEMENTED*/
		/* Player waits to be told what player he is (i.e. server sends him a playerID) */

		SDLNet_TCP_Recv(clientSocket, idBuff, 2);

		playerID = atoi(idBuff);

		int numused = SDLNet_TCP_AddSocket(clientSet, clientSocket);

		if (numused == -1 || numused == 0) 
		{
	    	printf("\n\n\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
	    	exit(2);
		}

		return playerID;
	}

	return -1;
}
//-------------------------------------------------------------------------------------
void Network::acceptClient()
{
	TCPsocket tmpAcceptSocket;

	tmpAcceptSocket = SDLNet_TCP_Accept(init_serverSocket);
 
	numberOfConnections++;
	char buff[2];
	sprintf(buff, "%d\0", numberOfConnections);

	// memcpy(buff, &val, 1);

	printf("accepting client as %s\n", buff);

	if (tmpAcceptSocket == NULL)
	{
		printf("CLIENT SOCKET NOT BEING ACCEPTED!\n\n\n");
		return;
	}

	SDLNet_TCP_Send(tmpAcceptSocket, buff, 2);
	clientIP = SDLNet_TCP_GetPeerAddress(tmpAcceptSocket);
	printf("Client connected - IP: %d\n\n", clientIP->host);

	int numused = SDLNet_TCP_AddSocket(clientSet, tmpAcceptSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}

	clientSocketList[numberOfConnections - 1] = tmpAcceptSocket;
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(char* pack, int socketID)
{
	int numSent;
	int packSize = getPacketSize(pack[0]);

	if (networkID == CLIENT)
		numSent = SDLNet_TCP_Send(clientSocket, pack, packSize);
	else if (networkID == SERVER)
	{
		S_PLAYER_packet s;
    	memcpy(&s, pack, sizeof(S_PLAYER_packet));
    	printf("\tS_packet Position: Ogre::Vector3(%f, %f, %f)\n", s.x, s.y, s.z);

		numSent = SDLNet_TCP_Send(clientSocketList[socketID], pack, packSize);
	}
	if (!numSent)
	{
		printf("*****Failed to send packets of size %d!\n\n", numSent);
	}
	else
		printf("*****Sending packet of size %d....\n\n", numSent);
}
//-------------------------------------------------------------------------------------
char* Network::receivePacket(int socketID)
{
	// we are going to recv packetList, no iterate thru buffer and return a packList
	char* buff = (char*)malloc(sizeof(char) * MAX_SIZE_OF_BUFFER);
	memset(buff, 0, MAX_SIZE_OF_BUFFER);

	int numRead;

	if (networkID == CLIENT)
	{
		if ((numRead = SDLNet_TCP_Recv(clientSocket, buff, MAX_SIZE_OF_BUFFER)) <= 0) 
		{
			//printf("Number of bytes read on misread: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER);
			return NULL;
		}
		printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER); 
		
		S_PLAYER_packet s;
    	memcpy(&s, buff, sizeof(S_PLAYER_packet));
    	printf("\t**********8S_packet Position: Ogre::Vector3(%f, %f, %f)\n", s.x, s.y, s.z);

		return buff;
	}
	else if (networkID == SERVER)
	{
		if ((numRead = SDLNet_TCP_Recv(clientSocketList[socketID], buff, MAX_SIZE_OF_BUFFER)) <= 0) 
		{
			printf("Number of bytes read on misread: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER);
			
			buff[0] = 'n';
		}
		printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER);
	}

	return buff;
}
//-------------------------------------------------------------------------------------
bool Network::checkSockets(int socketID)
{
	if (networkID == CLIENT)
	{
		if (SDLNet_CheckSockets(clientSet, 0) <= 0)
			return false;
		else 
		{
			printf("CHECKING SOCKET!\n\n");
			return true;
		}
	}
	else if (socketID >= 0)
	{
		if (SDLNet_SocketReady(clientSocketList[socketID]) <= 0)
			return false;
		else
			return true;
	}
	else if (socketID == -1)
	{
		if (SDLNet_CheckSockets(iSet, 0) <= 0)
			return false;
		else 
			return true;	
	}
}
//-------------------------------------------------------------------------------------
int Network::getPacketSize(char type)
{
	if (type == (char)(((int)'0') + C_PLAYER))
		return sizeof(C_PLAYER_packet);
	if (type == (char)(((int)'0') + S_PLAYER))
		return sizeof(S_PLAYER_packet);
	if (type == (char)(((int)'0') + DISK))
		return sizeof(DISK_packet);
	if (type == (char)(((int)'0') + INPUT))
		return sizeof(INPUT_packet);
	if (type == (char)(((int)'0') + POWERUP))
		return sizeof(POWERUP_packet);
	if (type == (char)(((int)'0') + TILE))
		return sizeof(TILE_packet);
	if (type == (char)(((int)'0') + GAMESTATE))
		return sizeof(GAMESTATE_packet);
	if (type == (char)(((int)'0') + EXPANSION))
		return sizeof(EXPANSION_packet);
}