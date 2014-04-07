#include "Network.h"

Network::Network(int sc_identifier, char* hostIP)
{
	numberOfConnections = 0;
	connectionEstablished = false;
	init_serverSocket = NULL;
	clientSocket = NULL;
	UDP_gameSocket = NULL;
	TCP_gameSocket = NULL;
	maxSizeOfList = sizeof(MCP_Packet) * 16;
	i_set = SDLNet_AllocSocketSet(5);

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
	SDLNet_FreeSocketSet(i_set);
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

	/* Add the socket to the socket i_set so we can use CheckSockets() later */
	int numused = SDLNet_TCP_AddSocket(i_set, init_serverSocket);

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
		char idBuff[5];

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

		SDLNet_TCP_Recv(clientSocket, idBuff, 5);

		playerID = atoi(idBuff);

		int numused = SDLNet_TCP_AddSocket(i_set, clientSocket);

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
	char buff[5];
	sprintf(buff, "%d", numberOfConnections);

	// memcpy(buff, &val, 1);

	printf("accepting client as %s\n", buff);

	if (tmpAcceptSocket == NULL)
	{
		printf("CLIENT SOCKET NOT BEING ACCEPTED!\n\n\n");
		return;
	}

	SDLNet_TCP_Send(tmpAcceptSocket, buff, 5);
	clientIP = SDLNet_TCP_GetPeerAddress(tmpAcceptSocket);
	printf("Client connected - IP: %d\n\n", clientIP->host);

	int numused = SDLNet_TCP_AddSocket(i_set, tmpAcceptSocket);

	if (numused == -1 || numused == 0) 
	{
    	printf("\n\n\nSDLNet_AddSocket: %s\n", SDLNet_GetError());
    	exit(2);
	}

	clientSocketList.push_back(tmpAcceptSocket);
}
//-------------------------------------------------------------------------------------
void Network::sendPacket(vector<MCP_Packet> packList, int socketID)
{
	int numSent;
	int packListSize = packList.size() * sizeof(MCP_Packet);
	char buff[maxSizeOfList];

	memset(buff, 0, maxSizeOfList); // bigger size

	for (int i = 0; i < packList.size(); i++)
		memcpy(buff+(i*sizeof(MCP_Packet)), &packList[i], sizeof(MCP_Packet));

	numSent = SDLNet_TCP_Send(clientSocketList[socketID], buff, packListSize);

	if (!numSent)
	{
		printf("*****Failed to send packets of size %d!\n\n", numSent);
	}
}
//-------------------------------------------------------------------------------------
vector<MCP_Packet> Network::receivePacket(int socketID)
{
	// we are going to recv packetList, no iterate thru buffer and return a packList
	vector<MCP_Packet> packList;
	MCP_Packet pack;

	char buff[maxSizeOfList]; 

	memset(buff, 0, maxSizeOfList);

	int numRead;

	if (socketID > 0)
	{
		if ((numRead = SDLNet_TCP_Recv(clientSocket, buff, maxSizeOfList)) <= 0) 
		{
			//printf("Number of bytes read on misread: %d\t\t max: %d\n\n", numRead, maxSizeOfList);
			pack.id = 'n';
			packList.push_back(pack);
			return packList;
		}

		//printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, maxSizeOfList);

		for (int i = 0; i < maxSizeOfList; i += sizeof(MCP_Packet))
		{
			memcpy(&pack, buff+i, sizeof(MCP_Packet));

			// printf("pack.id = %c\n\n", pack.id);
			if (pack.id == 'n')
				break;
			packList.push_back(pack);
		}
	}
	if (socketID == 0)
	{
		for (int i = 1; i < clientSocketList.size() + 1; i++)
		{
			if (SDLNet_SocketReady(clientSocketList[i]))
			{
				if ((numRead = SDLNet_TCP_Recv(TCP_gameSocket, buff, maxSizeOfList)) <= 0) 
				{
					printf("Number of bytes read on misread: %d\t\t max: %d\n\n", numRead, maxSizeOfList);
					pack.id = 'n';
					packList.push_back(pack);
					return packList;
				}

				printf("Number of bytes read: %d\t\t max: %d\n\n", numRead, maxSizeOfList);

				for (int i = 0; i < maxSizeOfList; i += sizeof(MCP_Packet))
				{
					memcpy(&pack, buff+i, sizeof(MCP_Packet));

					// printf("pack.id = %c\n\n", pack.id);
					if (pack.id == 'n')
						break;
					packList.push_back(pack);
				}
			}
		}	
	}

	return packList;
}
//-------------------------------------------------------------------------------------
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
