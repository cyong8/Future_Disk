#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	numberOfConnections = 0;
	initServerSocket = NULL;
	clientSocket = NULL;
	connections = vector<Connection>(MAX_NUMBER_OF_PLAYERS);
	clientSet = SDLNet_AllocSocketSet(MAX_NUMBER_OF_PLAYERS + 1);

	if (clientSet == NULL)
	{
		printf("Error allocating socket set\n\n\n");
		exit(1);
	}

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
	SDLNet_TCP_Close(initServerSocket);
	SDLNet_TCP_Close(clientSocket);
	SDLNet_FreeSocketSet(clientSet);
}
//-------------------------------------------------------------------------------------
void Network::startListening()
{
	/* Initialize listening of the host for clients */
	if (SDLNet_ResolveHost(&serverIP, NULL, TCP_portNum) < 0) // NULL indicates listening
	{
	    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
	    exit(1);
	}
	/* Open the TCP connection with the server IP found above */
	initServerSocket = SDLNet_TCP_Open(&serverIP);	

	if(!initServerSocket) 
	{
	    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
	    exit(2);
	}

	/* Add the socket to the socket clientSet so we can use CheckSockets() later */
	int numused = SDLNet_TCP_AddSocket(clientSet, initServerSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}
}
//-------------------------------------------------------------------------------------
int Network::establishConnection()
{
	if (networkID == SERVER) 
	{
		if (SDLNet_SocketReady(initServerSocket))
		{
			acceptClient();
			return 1;
		}
	}
	else if (networkID == CLIENT)
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
	int socketIndex = numberOfConnections;

	tmpAcceptSocket = SDLNet_TCP_Accept(initServerSocket);

	if (tmpAcceptSocket == NULL)
	{
		printf("CLIENT SOCKET NOT BEING ACCEPTED!\n\n\n");
		exit(1);
	}
 
 	connections[socketIndex].sock = tmpAcceptSocket;
	
	numberOfConnections++;
	char buff[2];
	sprintf(buff, "%d\0", numberOfConnections);

	clientIP = SDLNet_TCP_GetPeerAddress(connections[socketIndex].sock);
	int numused = SDLNet_TCP_AddSocket(clientSet, connections[socketIndex].sock);
	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}

	SDLNet_TCP_Send(connections[socketIndex].sock, buff, 2);
}
//-------------------------------------------------------------------------------------
int Network::sendPacket(char* pack, int socketIndex)
{
	int numSent = 0;
	int packSize = getPacketSize(pack[0]);

	/********************************* CLIENT *********************************/
	if (networkID == CLIENT)
	{
		C_PLAYER_packet p;
		memcpy(&p, pack, sizeof(C_PLAYER_packet));

		// printf("\tPacket ID: %c\n", p.packetID);
		// printf("\tPlayer ID: %c\n", p.playID);
		numSent = SDLNet_TCP_Send(clientSocket, pack, packSize);
	}
	/********************************* SERVER *********************************/
	if (networkID == SERVER)
	{
		S_PLAYER_packet p;
        memcpy(&p, pack, sizeof(S_PLAYER_packet));
        printf("\tUpdate of Player%c\n", p.playID);
		numSent = SDLNet_TCP_Send(connections[socketIndex].sock, pack, packSize);
	}
	return numSent;
	
	// if (!numSent) /* ERROR CHECK */
	// 	printf("*****Failed to send packets of size %d!\n\n", numSent);
	// else
	// 	printf("*****Sending packet of size %d....\n\n", numSent);
}
//-------------------------------------------------------------------------------------
char* Network::receivePacket(int socketIndex)
{
	// we are going to recv packetList, no iterate thru buffer and return a packList
	char* buff = (char*)malloc(sizeof(char) * MAX_SIZE_OF_BUFFER);
	memset(buff, 0, MAX_SIZE_OF_BUFFER);

	int numRead;
	/********************************* CLIENT *********************************/
	if (networkID == CLIENT)
	{
		numRead = SDLNet_TCP_Recv(clientSocket, buff, MAX_SIZE_OF_BUFFER);
		printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER);
	}
	/********************************* SERVER *********************************/
	if (networkID == SERVER)
	{
		numRead = SDLNet_TCP_Recv(connections[socketIndex].sock, buff, MAX_SIZE_OF_BUFFER);
	}
	// printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER);

	if (numRead <= 0) 
		return NULL;

	return buff;
}
//-------------------------------------------------------------------------------------
bool Network::checkSockets(int socketIndex)
{
	if (networkID == CLIENT)
	{
		if (SDLNet_CheckSockets(clientSet, 0))
		{
			if (SDLNet_SocketReady(clientSocket))
				return true;
			else
				return false;
		}
	}
	else if (networkID == SERVER)
	{
		if (socketIndex == -1)
		{
			if (SDLNet_CheckSockets(clientSet, 0))
			{
				if (SDLNet_SocketReady(initServerSocket))
				{
					// printf("*****ACTIVITY ON SERVER SOCKET\n\n");
					return true;
				}
			}
		}
		else if (socketIndex >= 0)
		{
			if (connections[socketIndex].sock != NULL)
			{
				if (SDLNet_SocketReady(connections[socketIndex].sock))
				{
					// printf("\t*******NETWORK RECEIVING PACKETS*******\n\n\n\n");
					return true;
				}
			}
			else 
				return false;
		}
	}
	
	return false;
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
//-------------------------------------------------------------------------------------
void Network::removeClient(int clientIndex)
{
	SDLNet_TCP_Close((connections[clientIndex].sock));
	connections[clientIndex].active = false;
}
//-------------------------------------------------------------------------------------