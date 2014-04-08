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

	timeSinceLastStateUpdate = 0.01f;
    mMove = 5.0f;
    mRotate = 0.1f;
    sprintFactor = 2.0f;
    gameDisk = NULL;
    numberOfClients = 0;

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
    gameRoom = new Room(sSceneMgr, gameSimulator, 0);

    /********************  OBJECT CREATION  ********************/
/*	Add Players when people connect

    (new Player("Player1", sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, 15.0f), "Positive Side"))->addToSimulator(); // Create Player 1
    (new Player("Player2", sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, -15.0f), "Negative Side"))->addToSimulator(); // Create Player 2
*/
    Power = new Target("Power", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Speed = new Target("Speed", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    JumpPower = new Target("Jump", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Restore = new Target("Restore", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());

    Power->addToSimulator();
    Speed->addToSimulator();
    JumpPower->addToSimulator();
    Restore->addToSimulator();

    sSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = sSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getWall(Ceiling)->getSceneNode()->getPosition().y, 0.0f));

    printf("about to add power ups!\n\n");
    // createOverlays(pCam); // in MCP
}
//-------------------------------------------------------------------------------------
bool Server::frameRenderingQueued(const Ogre::FrameEvent& evt) // listen only on socketSet index 0
{
    // else if (!gameStart) // may need
    //     gameStart = true;

    if (!processUnbufferedInput(evt))
        exit(2);

    // gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f);
    // gameSimulator->parseCollisions(); // check collisions


    if (gameNetwork->checkSockets()) //&& clientGameStart)
    {
        if (gameNetwork->establishConnection() == 1) // attempt to establish a connection at each frame
        {
            // Add the given player to the scene and simulator
            numberOfClients++;
            printf("FOUND A CLIENT!!!\n\n\n");
            // Client has connected; give it a player
            char playerBuffer[25];
            sprintf(playerBuffer, "Player%d", numberOfClients);

            playerList.push_back(new Player(playerBuffer, sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), numberOfClients));
            playerList[numberOfClients-1]->addToSimulator();
        }

        int i = 0;
        vector<MCP_Packet> packList;
        packList = gameNetwork->receivePacket(0);
       
        while (packList.size() > i && packList[i].packetID != 'n')
        {
            printf("pack.packetID (inside while) = %c\n\n", packList[i].packetID);
            interpretClientPacket(packList[i]);
            i++;
        }
    }        

    updateRemovedTiles();
    // if (mShutDown)
           // exit(2);                  

    for (int i = 0; i < numberOfClients; i++)
    {
        updateClientVelocity(playerList[i]);
        restrictPlayerMovement(playerList[i]);

        if (timeSinceLastStateUpdate < 0.0f)
            constructAndSendGameState(i);

         /* DEBUGGING */
        // printf("\t\t Client States: W - %d\n", p1->checkState(Forward));
        // printf("\t\t Client States: A - %d\n", p1->checkState(Left));
        // printf("\t\t Client States: S - %d\n", p1->checkState(Back));
        // printf("\t\t Client States: D - %d\n", p1->checkState(Right));
        // printf("\t\t Client States: Shift - %d\n", p1->checkState(Boost));
    }

    if (gameSimulator->setDisk && gameSimulator->gameDisk == NULL)
    {
        gameDisk = new Disk("Disk", sSceneMgr, gameSimulator, 0.0f/*Ogre::Math::RangeRandom(0,2)*/);
        gameDisk->addToSimulator();
    }

    if (timeSinceLastStateUpdate < 0.0f)
        timeSinceLastStateUpdate = 0.01f;

    timeSinceLastStateUpdate -= evt.timeSinceLastFrame;
}
//-------------------------------------------------------------------------------------
bool Server::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
	return true;
}
//-------------------------------------------------------------------------------------
void Server::updateClientVelocity(Player* p)
{
	if (gameSimulator->checkGameStart()) //&& !clientVKeyDown) // Need to establish new way of checking View Mode
    {
        Ogre::Vector3 velocityVector;
        velocityVector = p->fillClientVelocityVector(mMove, sprintFactor); // p1 was hostPlayer
        velocityVector = p->getSceneNode()->getOrientation() * velocityVector;  // p2 was clientPlayer
        btVector3 btTrueVelocity = btVector3(velocityVector.x, velocityVector.y, velocityVector.z);
        // No longer named clientPlayer/hostPlayer - receive player id in packet
        // clientPlayer->getBody()->setLinearVelocity(btTrueVelocity + (btVector3(0.0f, clientPlayer->getBody()->getLinearVelocity().getY(), 0.0f)));
    }
}
//-------------------------------------------------------------------------------------
bool Server::constructAndSendGameState(int socketID)
{
	vector<MCP_Packet> packList;

    for (int i = 0; i < numberOfClients; i++)
    {
        PLAYER_packet pack;
        memset(&pack, 0, sizeof(PLAYER_packet));

        pack.packetID = (char)(((int)'0') + PLAYER);
        pack.playID = (char)(((int)'0') + i);
        pack.x = playerList[i]->getSceneNode()->getPosition().x;
        pack.y = playerList[i]->getSceneNode()->getPosition().y;
        pack.z = playerList[i]->getSceneNode()->getPosition().z;
        pack.orientation = playerList[i]->getSceneNode()->getOrientation();

        packList.push_back(pack);
    }

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
        DISK_packet pack;

        pack.packetID = (char)(((int)'0') + DISK);
        pack.diskID = (char)((int)'0');
        pack.x = gameDisk->getSceneNode()->_getDerivedPosition().x;
        pack.y = gameDisk->getSceneNode()->_getDerivedPosition().y;
        pack.z = gameDisk->getSceneNode()->_getDerivedPosition().z;
        pack.orientation = gameDisk->getSceneNode()->_getDerivedOrientation();

        packList.push_back(pack);
    }
    if (gameSimulator->checkGameStart())
    {
        GAMESTATE_packet pack;
        pack.packetID = (char)(((int)'0') + GAMESTATE);   
        packList.push_back(pack);
    }

    MCP_Packet pack;

    pack.packetID = 'n';
    packList.push_back(pack);

    gameNetwork->sendPacket(packList, socketID);
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
    Ogre::SceneNode* restrictNode;
    Ogre::AxisAlignedBox gapBox;
    Ogre::AxisAlignedBox playerBox = p->getSceneNode()->_getWorldAABB();

    if (p->getPlayerSide() == "Negative Side")
        restrictNode = gameRoom->getClientGapSceneNode();
    else
        restrictNode = gameRoom->getHostGapSceneNode();

    gapBox = restrictNode->_getWorldAABB();

    if (gapBox.intersects(playerBox))
    {
        time(&gapStartTime);
        time(&gapEndTime);
        // gapStartClock = clock();

        p->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        p->getBody()->setLinearVelocity(btVector3(velocityVector.getX(), velocityVector.getY(), 5.0f));
        restrictNode->setVisible(true);
        p->setMovementRestriction(true);
    }
    else
    {
        time(&gapEndTime);
        // gapEndClock = clock();

        if (difftime(gapEndTime, gapStartTime) > 1.0f)
        {
            restrictNode->setVisible(false);
        }

        p->setMovementRestriction(false);
    }
}
//-------------------------------------------------------------------------------------
bool Server::interpretClientPacket(MCP_Packet pack)
{
    // Update the player rigid body and scenenode - Note: The states[] of the host tracks the client state; not the host state
    char typeInput = pack.packetID;
    
//    printf("\t\t*****Client sending sequence %c\n\n", pack.id);
    
    if (typeInput == 'w')                                       // Forward
    {
        // if (hostPlayer->checkState(Forward))
        //     hostPlayer->toggleState(Forward, false);
        // else
        //     hostPlayer->toggleState(Forward, true);
    }
    if (typeInput == 'a')                                       // Left
    {
        // if (hostPlayer->checkState(Left))
        //     hostPlayer->toggleState(Left, false);
        // else
        //     hostPlayer->toggleState(Left, true);
    }
    if (typeInput == 's')                                       // Backwards
    {
        // if (hostPlayer->checkState(Back))
        //     hostPlayer->toggleState(Back, false);
        // else
        //     hostPlayer->toggleState(Back, true);
    }
    if (typeInput == 'd')                                       // Right
    {
        // if (hostPlayer->checkState(Right))
        //     hostPlayer->toggleState(Right, false);
        // else
        //     hostPlayer->toggleState(Right, true);
    }
    if (typeInput == 'j') //&& !clientPlayer->groundConstantSet)   // Jump
    {
        // clientPlayer->performJump();
    }
    if (typeInput == 'v')                                       // View Mode Toggle
    {
        // clientVKeyDown = !clientVKeyDown;
    }
    if (typeInput == 'b')                                       // speed boost
    {
        // if (hostPlayer->checkState(Boost))
        //     hostPlayer->toggleState(Boost, false);
        // else
        //     hostPlayer->toggleState(Boost, true);
    }
    if (typeInput == 't') //&& clientPlayer->checkHolding())       // Player tried to throw
    {
        gameSimulator->setThrowFlag();
    }
    if (typeInput == 'o')
    {
        // clientPlayer->getSceneNode()->_setDerivedOrientation(pack.orientationQ);

        // btQuaternion rotationQ;
        // btTransform transform = clientPlayer->getBody()->getCenterOfMassTransform();

        // rotationQ = btQuaternion(clientPlayer->getSceneNode()->getOrientation().getYaw().valueRadians(), 0, 0);
        // transform.setRotation(rotationQ);

        // clientPlayer->getBody()->setCenterOfMassTransform(transform);
    }
    if (typeInput == 'S')
    {
        // clientPlayer->getPlayerSightNode()->setPosition(Ogre::Vector3(pack.x_coordinate, pack.y_coordinate, pack.z_coordinate));
    }

    return false;
}
//-------------------------------------------------------------------------------------
void Server::keyPressed(const OIS::KeyEvent &evt)
{

}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------