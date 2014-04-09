#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	numberOfConnections = 0;
	connectionEstablished = false;
	init_serverSocket = NULL;
	clientSocket = NULL;
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
	    	return -1;
		}

		clientSocket = SDLNet_TCP_Open(&serverIP);

		if(!clientSocket) 
		{
		    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		    return -1;
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
		clientSocketList.push_back(clientSocket);

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

	clientSocketList.push_back(tmpAcceptSocket);
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(char* packList, int socketID)
{
	int numSent;

	// memset(buff, 0, MAX_SIZE_OF_BUFFER); // bigger size

	// printf("Size of Packet List to be sent: %d\n\n", (int)packList.size());
	// for (int i = 0; i < packList.size(); i++)
	// {
	// 	if (packList[i].packetID == (char)(((int)'0') + PLAYER))
	// 		memcpy(buff+(i*sizeof(PLAYER_packet)), &packList[i], sizeof(PLAYER_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + DISK))
	// 		memcpy(buff+(i*sizeof(DISK_packet)), &packList[i], sizeof(DISK_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + INPUT))
	// 		memcpy(buff+(i*sizeof(INPUT_packet)), &packList[i], sizeof(INPUT_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + POWERUP))
	// 		memcpy(buff+(i*sizeof(POWERUP_packet)), &packList[i], sizeof(POWERUP_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + TILE))
	// 		memcpy(buff+(i*sizeof(TILE_packet)), &packList[i], sizeof(TILE_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + GAMESTATE))
	// 		memcpy(buff+(i*sizeof(GAMESTATE_packet)), &packList[i], sizeof(GAMESTATE_packet));
	// 	else if (packList[i].packetID == (char)(((int)'0') + EXPANSION))
	// 		memcpy(buff+(i*sizeof(EXPANSION_packet)), &packList[i], sizeof(EXPANSION_packet));
	// }

	if (networkID == CLIENT)
		numSent = SDLNet_TCP_Send(clientSocket, packList, MAX_SIZE_OF_BUFFER);
	else if (networkID == SERVER)
		numSent = SDLNet_TCP_Send(clientSocketList[socketID], packList, MAX_SIZE_OF_BUFFER);

	if (!numSent)
	{
		printf("*****Failed to send packets of size %d!\n\n", numSent);
	}
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
			buff[0] = 'n';
		}
		return buff;
		//printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, MAX_SIZE_OF_BUFFER); 
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
		if (SDLNet_SocketReady(clientSocket) <= 0)
			return false;
		else 
			return true;
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
