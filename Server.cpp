#include "Server.h"

Server::Server(MCP* mcp)//Music* mus, Ogre::SceneManager* mgr)
{
	MasterControl = mcp;
	gameMusic = mcp->getMusicObject();
	sSceneMgr = mcp->getSceneManager();

	gameNetwork = new Network(SERVER, 0);
	gameSimulator = new Simulator(sSceneMgr, gameMusic);

    srand(time(0));
    time(&gapStartTime);
    time(&gapEndTime);
    
    mMove = 5.0f;
    mRotate = 0.1f;
    sprintFactor = 2.0f;
    gameDisk = NULL;
    numberOfClients = 0;

    playerList = vector<Player*>(MAX_NUMBER_OF_PLAYERS, NULL);

    printf("Create host scene!\n\n");

    createScene();
}
//-------------------------------------------------------------------------------------
Server::~Server(void)
{
}
//-------------------------------------------------------------------------------------
void Server::createScene()
{
    gameRoom = new Room(sSceneMgr, gameSimulator, 0, 2);
    gameSimulator->setFloorY(gameRoom->getFloorPositionY());

    sSceneMgr->getCamera("PlayerCam")->lookAt(gameSimulator->getGameObject("Ceiling")->getSceneNode()->getPosition());

    /********************  POWER UPS  ********************/
    // Power = new Target("Power", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f));
    // Speed = new Target("Speed", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f));
    // JumpPower = new Target("Jump", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f));
    // Restore = new Target("Restore", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f));

    // Power->addToSimulator();
    // Speed->addToSimulator();
    // JumpPower->addToSimulator();
    // Restore->addToSimulator();

    sSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    directLight = sSceneMgr->createLight("directionalLight");  // Point light
    directLight->setType(Ogre::Light::LT_POINT); // change to directional light
    directLight->setDiffuseColour(Ogre::ColourValue::White);
    directLight->setSpecularColour(Ogre::ColourValue::White);
    directLight->setVisible(true);
    directLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getWall(Ceiling)->getSceneNode()->getPosition().y, 0.0f));
    // directLight->setDirection(Ogre::Vector3( 0, -1, 0));
    
    updateClock = clock();
}
//-------------------------------------------------------------------------------------
bool Server::frameRenderingQueued(Ogre::Real tSinceLastFrame) // listen only on socketSet index 0
{
    gameSimulator->stepSimulation(tSinceLastFrame, 1, 1.0f/120.0f);
    gameSimulator->parseCollisions(); // check collisions

    if (gameNetwork->checkSockets(-1)) // CHECK FOR NEW PLAYERS
    {
        if (gameNetwork->establishConnection() == 1) // attempt to establish a connection at each frame
        {
            // Add the given player to the scene and simulator
            numberOfClients++;
            printf("FOUND A CLIENT!!!\n\n\n");
            // Client has connected; give it a player
            char playerBuffer[25];
            sprintf(playerBuffer, "Player%d", numberOfClients);

            Player *newP = new Player(playerBuffer, sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), numberOfClients, Ogre::Vector3(gameRoom->getWidth(), gameRoom->getHeight(), (Ogre::Real)gameRoom->getNumberOfPlayers()));
            playerList[numberOfClients-1] = newP;
            playerList[numberOfClients-1]->addToSimulator();
        }
    }
    updateRemovedTiles();

    for (int i = 0; i < numberOfClients; i++)  // CHECK FOR ACTIVITY FROM CURRENT PLAYERS
    {
        if (gameNetwork->checkSockets(i))
            interpretClientPacket(i);

        updateClientVelocity(playerList[i]);
        // restrictPlayerMovement(playerList[i]);

    }
    
    if (((float)(clock() - updateClock))/CLOCKS_PER_SEC  > 0.01f)
    {
        constructAndSendGameState();
        updateClock = clock();
    }
    
    if (gameSimulator->setDisk && gameSimulator->gameDisk == NULL && numberOfClients > 1)
    {
        gameDisk = new Disk("Disk", sSceneMgr, gameSimulator, 0.0f/*Ogre::Math::RangeRandom(0,2)*/);
        gameDisk->addToSimulator();
    }    
}
//-------------------------------------------------------------------------------------
void Server::updateClientVelocity(Player* p)
{
	// if (gameSimulator->checkGameStart()) //&& !clientVKeyDown) // Need to establish new way of checking View Mode
    // {
        Ogre::Vector3 velocityVector;
        velocityVector = p->fillVelocityVector(mMove, sprintFactor);
        velocityVector = p->getSceneNode()->getOrientation() * velocityVector;
        btVector3 btTrueVelocity = btVector3(velocityVector.x, velocityVector.y, velocityVector.z);
        // No longer named clientPlayer/hostPlayer - receive player id in packet
        p->getBody()->setLinearVelocity(btTrueVelocity + (btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f)));
    // }
}
//-------------------------------------------------------------------------------------
bool Server::constructAndSendGameState() /* MOVE LOOPED CALL OF THIS FUNCTION TO THIS FUNCTION; CONSTRUCT PACKETS ONCE INSTEAD OF N */
{
    char* buff;
    int totalBytesSent = 0;
    buff = (char*)malloc(sizeof(S_PLAYER_packet));
    vector<S_PLAYER_packet> packList; 

    /* Sending each player's position to clients */
    for (int i = 0; i < numberOfClients; i++)
    {
        S_PLAYER_packet pack;
     
        memset(&pack, 0, sizeof(S_PLAYER_packet));

        pack.packetID = (char)(((int)'0') + S_PLAYER);
        pack.playID = (char)(((int)'0') + (i + 1));

        // printf("\n\nconstructing PLAYER_packet: packetID = %c, playID = %c\n\n", pack.packetID, pack.playID);
        pack.x = playerList[i]->getSceneNode()->_getDerivedPosition().x;
        pack.y = playerList[i]->getSceneNode()->_getDerivedPosition().y;
        pack.z = playerList[i]->getSceneNode()->_getDerivedPosition().z;
        // printf("\tPack Position: Ogre::Vector3(%f, %f, %f)\n", pack.x, pack.y, pack.z);
        pack.orientation = playerList[i]->getSceneNode()->_getDerivedOrientation();
        packList.push_back(pack);
    }

    /* UPDATE ACTIVE POWER UPS */

    // if ()
    // pack.id = 'P';
    // pack.x_coordinate = Power->getSceneNode()->_getDerivedPosition().x;
    // pack.y_coordinate = Power->getSceneNode()->_getDerivedPosition().y;
    // pack.z_coordinate = Power->getSceneNode()->_getDerivedPosition().z;
    // packList.push_back(pack);

    // pack.id = 'S';
    // pack.x_coordinate = Speed->getSceneNode()->_getDerivedPosition().x;
    // pack.y_coordinate = Speed->getSceneNode()->_getDerivedPosition().y;
    // pack.z_coordinate = Speed->getSceneNode()->_getDerivedPosition().z;
    // packList.push_back(pack);

    // pack.id = 'J';
    // pack.x_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().x;
    // pack.y_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().y;
    // pack.z_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().z;
    // packList.push_back(pack);

    // pack.id = 'R';
    // pack.x_coordinate = Restore->getSceneNode()->_getDerivedPosition().x;
    // pack.y_coordinate = Restore->getSceneNode()->_getDerivedPosition().y;
    // pack.z_coordinate = Restore->getSceneNode()->_getDerivedPosition().z;
    // packList.push_back(pack);

    // int rIndex;
    // while (removedCTileList.size() != 0)
    // {
    //     pack.id = 'C';

    //     rIndex = removedCTileList[removedCTileList.size() - 1];
    //     removedCTileList.pop_back();

    //     pack.tileIndex = rIndex;
    //     packList.push_back(pack);
    // }
    // while (removedHTileList.size() != 0)
    // {
    //     pack.id = 'H';   

    //     rIndex = removedHTileList[removedHTileList.size() - 1];
    //     removedHTileList.pop_back();

    //     pack.tileIndex = rIndex;
    //     packList.push_back(pack);
    // }
    if (gameDisk != NULL)
    {
        buff = (char*)malloc(sizeof(DISK_packet));

        DISK_packet pack;
        memset(&pack, 0, sizeof(DISK_packet));

        pack.packetID = (char)(((int)'0') + DISK);
        pack.diskID = (char)((int)'0');
        pack.x = gameDisk->getSceneNode()->_getDerivedPosition().x;
        pack.y = gameDisk->getSceneNode()->_getDerivedPosition().y;
        pack.z = gameDisk->getSceneNode()->_getDerivedPosition().z;
        pack.orientation = gameDisk->getSceneNode()->_getDerivedOrientation();

        memcpy(buff, &pack, sizeof(DISK_packet));
        // gameNetwork->sendPacket(buff, clientIndex);
    }
    // if (gameSimulator->checkGameStart()) // Don't want to do every frame
    // {
    //     GAMESTATE_packet pack;
    //     memset(&pack, 0, sizeof(GAMESTATE_packet));

    //     pack.packetID = (char)(((int)'0') + GAMESTATE);   
    //     memcpy(buff + indexIntoBuff, &pack, sizeof(GAMESTATE_packet));
    //     indexIntoBuff += sizeof(GAMESTATE_packet);
    // }
    for (int i = 0; i < numberOfClients; i++)
    {
        memcpy(buff, &packList[i], sizeof(S_PLAYER_packet));

        for (int j = 0; j < numberOfClients; j++)
        {
            if (playerList[j] != NULL)
                gameNetwork->sendPacket(buff, j);
        }
    }

    // printf("*****Sending packets to Client%d of size %d....\n\n", 1, totalBytesSent);
}
//-------------------------------------------------------------------------------------
void Server::updateRemovedTiles() // HACKED
{
    if (removedHTileList.size() != gameSimulator->hostRemoveIndexes.size() && gameSimulator->newRemovedTile)
    {
        int i = removedHTileList.size() - 1;

        while (i != (gameSimulator->hostRemoveIndexes.size()))
        {
            removedHTileList.push_back(gameSimulator->hostRemoveIndexes[i]);
            i++;
        }
        gameSimulator->newRemovedTile = false;
    }
    if (removedCTileList.size() != gameSimulator->clientRemoveIndexes.size() && gameSimulator->newRemovedTile)
    {
        int i = removedCTileList.size() - 1;

        while (i != (gameSimulator->clientRemoveIndexes.size()))
        {
            removedCTileList.push_back(gameSimulator->clientRemoveIndexes[i]);
            i++;
        }

        gameSimulator->newRemovedTile = false;
    }
}
//-------------------------------------------------------------------------------------
void Server::restrictPlayerMovement(Player* p)
{
    Ogre::Vector3 pos = p->getSceneNode()->getPosition();
    Ogre::Vector3 dim = p->getPlayerDimensions();
    btVector3 velocityVector = p->getBody()->getLinearVelocity();
    Ogre::SceneNode* restrictHNode;
    Ogre::SceneNode* restrictVNode;
    Ogre::AxisAlignedBox gapHBox;
    Ogre::AxisAlignedBox gapVBox;
    Ogre::AxisAlignedBox playerBox = p->getSceneNode()->_getWorldAABB();
    Ogre::Real pushBackVelocity = 5.0f;

    Gap* gp = gameRoom->getPlayerGapSceneNode(p->getPlayerID());

    restrictHNode = gp->hGap;
    restrictVNode = gp->vGap;

    gapHBox = restrictHNode->_getWorldAABB();
    gapVBox = restrictVNode->_getWorldAABB();


    if (gapHBox.intersects(playerBox))
    {
        time(&gapStartTime);
        time(&gapEndTime);

        if (p->getPlayerID() > 2)
            pushBackVelocity = -pushBackVelocity;

        p->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        p->getBody()->setLinearVelocity(btVector3(velocityVector.getX(), velocityVector.getY(), pushBackVelocity));
        restrictHNode->setVisible(true);
        p->setMovementRestriction(true);
    }
    else if (gapVBox.intersects(playerBox))
    {
        time(&gapStartTime);
        time(&gapEndTime);

        if (p->getPlayerID() == 1 || p->getPlayerID() == 3)
            pushBackVelocity = -pushBackVelocity;

        p->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        p->getBody()->setLinearVelocity(btVector3(pushBackVelocity, velocityVector.getY(), velocityVector.getZ()));
        restrictVNode->setVisible(true);
        p->setMovementRestriction(true);
    }
    else
    {
        time(&gapEndTime);
        // gapEndClock = clock();

        if (difftime(gapEndTime, gapStartTime) > 1.0f)
        {
            restrictHNode->setVisible(false);
            restrictVNode->setVisible(false);
        }

        p->setMovementRestriction(false);
    }
}
//-------------------------------------------------------------------------------------
bool Server::interpretClientPacket(int playerID)
{
    int indexIntoBuff = 0;
    char* buff = gameNetwork->receivePacket(playerID);
    
    if (buff == NULL)
        return false;

    printf("*****Client sending sequence\n\n");
    while (indexIntoBuff < MAX_SIZE_OF_BUFFER && buff[indexIntoBuff] != 0x00)
    {
        int packetID = buff[indexIntoBuff] - '0';
        printf("\tpacketID = %d\n", packetID);

        if (packetID == INPUT)
        {
            INPUT_packet i;

            memcpy(&i, buff+indexIntoBuff, sizeof(INPUT_packet));

            // interpret i
            int pID = i.playID - '0';
            // keyID inputID = static_cast<keyID>(i.key - '0');
            printf("PROCESSING CLIENT INPUT: %c\n", i.key);
            processClientInput((pID - 1), i.key);

            indexIntoBuff += sizeof(INPUT_packet);
        }
        else if (packetID == C_PLAYER)
        {   
            C_PLAYER_packet p;
            printf("FOUND CLIENT PLAYER PACKET!!!\n\n\n");
            memcpy(&p, buff+indexIntoBuff, sizeof(C_PLAYER_packet));

            int pID = p.playID - '0';
            Ogre::Quaternion quat = p.orientation;

            // interpret p
            Player* cp = playerList[pID-1];

            cp->getSceneNode()->_setDerivedOrientation(p.orientation);

            btQuaternion rotationQ;
            btTransform transform = cp->getBody()->getCenterOfMassTransform();
            rotationQ = btQuaternion(p.orientation.getYaw().valueRadians(), 0, 0);
            transform.setRotation(rotationQ);

            cp->getBody()->setCenterOfMassTransform(transform);

            indexIntoBuff += sizeof(C_PLAYER_packet);
        }
        // else if (packetID == GAMESTATE)
        // {
        //     GAMESTATE_packet g;

        //     memcpy(&g, buff+indexIntoBuff, sizeof(GAMESTATE_packet));

        //     // interpret g

        //     indexIntoBuff += sizeof(GAMESTATE_packet);
        // }
    }
    // if (typeInput == 'j') //&& !clientPlayer->groundConstantSet)   // Jump
    // {
    //     // clientPlayer->performJump();
    // }
    // if (typeInput == 'v')                                       // View Mode Toggle
    // {
    //     // clientVKeyDown = !clientVKeyDown;
    // }
    // if (typeInput == 'b')                                       // speed boost
    // {
    //     // if (hostPlayer->checkState(Boost))
    //     //     hostPlayer->toggleState(Boost, false);
    //     // else
    //     //     hostPlayer->toggleState(Boost, true);
    // }
    // if (typeInput == 't') //&& clientPlayer->checkHolding())       // Player tried to throw
    // {
    //     gameSimulator->setThrowFlag();
    // }
    // if (typeInput == 'o')
    // {
    //     // clientPlayer->getSceneNode()->_setDerivedOrientation(pack.orientationQ);

    //     // btQuaternion rotationQ;
    //     // btTransform transform = clientPlayer->getBody()->getCenterOfMassTransform();

    //     // rotationQ = btQuaternion(clientPlayer->getSceneNode()->getOrientation().getYaw().valueRadians(), 0, 0);
    //     // transform.setRotation(rotationQ);

    //     // clientPlayer->getBody()->setCenterOfMassTransform(transform);
    // }
    // if (typeInput == 'S')
    // {
    //     // clientPlayer->getPlayerSightNode()->setPosition(Ogre::Vector3(pack.x_coordinate, pack.y_coordinate, pack.z_coordinate));
    // }

    return false;
}
//-------------------------------------------------------------------------------------
void Server::processClientInput(int playerIndex, char keyPressed)
{
    Player* p = playerList[playerIndex];

    switch(keyPressed)
    {
        case 'w':
            if (p->checkState(FORWARD))
                p->setState(FORWARD, false);
            else
                p->setState(FORWARD, true);
            break;
        case 'a':
            if (p->checkState(LEFT))
                p->setState(LEFT, false);
            else
                p->setState(LEFT, true);
            break;
        case 's':
            if (p->checkState(BACK))
                p->setState(BACK, false);
            else
                p->setState(BACK, true);
            break;
        case 'd':
            if (p->checkState(RIGHT))
                p->setState(RIGHT, false);
            else
                p->setState(RIGHT, true);
            break;
        case 'j': // ATTEMPT TO PERFORM JUMP
            p->performJump();
            break;
        case 'b': 
            if (p->checkState(BOOST))
                p->setState(BOOST, false);
            else
                p->setState(BOOST, true);
            break;
        case 'm':  // PERFORM THROW IN SIMULATOR HANDLES IF THROW FLAG IS SET
            break;
        case 'q':  // GAME STATE CHANGE
            break;
        case 'k':// ENTER: // GAME STATE CHANGE
            break;
    }
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------