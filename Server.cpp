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
    pseudoHostStartGame = false;
    gameRoomCreated = false;
    powerUpsSpawned = false;

    gameDisk = NULL;
    numberOfClients = 0;
    forceUpdate = false;

    playerList = vector<Player*>(MAX_NUMBER_OF_PLAYERS, NULL);
    playerGameStatesList = vector<playerGameStates>(MAX_NUMBER_OF_PLAYERS, DISCONNECTED);

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
    twoPlayerGameRoom = new Room(sSceneMgr, gameSimulator, 2);
    fourPlayerGameRoom = new Room(sSceneMgr, gameSimulator, 4);

    sSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    directLight = sSceneMgr->createLight("roomLight");
    directLight->setType(Ogre::Light::LT_POINT);            // change to directional light
    directLight->setDiffuseColour(Ogre::ColourValue::White);
    directLight->setSpecularColour(Ogre::ColourValue::White);
    directLight->setVisible(true);

    twoPlayerGameRoom->activateRoom();
    activeRoom = twoPlayerGameRoom;
    
    /********************  POWER UPS  ********************/
    Target* Explosive;
    Target* Speed;
    Target* JumpPower;
    Target* Restore;
    for (int i = 1; i <= MAX_NUMBER_OF_PLAYERS; i++)
    {
        Explosive = new Target("Explosive_" + Ogre::StringConverter::toString(i), sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, EXPLOSIVE, i-1);
        explosiveList.push_back(Explosive);

        Speed = new Target("Speed_" + Ogre::StringConverter::toString(i), sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, SPEED, i-1);
        speedList.push_back(Speed);

        JumpPower = new Target("Jump_" + Ogre::StringConverter::toString(i), sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, JUMPBOOST, i-1);
        jumpList.push_back(JumpPower);

        Restore = new Target("Restore_" + Ogre::StringConverter::toString(i), sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, RESTORE, i-1);
        restoreList.push_back(Restore);
    }
    
    sSceneMgr->getCamera("PlayerCam")->lookAt(activeRoom->getWall(Ceiling)->getSceneNode()->getPosition());

    updateClock = clock();
}
//-------------------------------------------------------------------------------------
bool Server::frameRenderingQueued(Ogre::Real tSinceLastFrame) // listen only on socketSet index 0
{
    if (pseudoHostStartGame)
    {
        gameSimulator->stepSimulation(tSinceLastFrame, 1, 1.0f/120.0f);
        gameSimulator->parseCollisions();

        if (gameSimulator->checkDiskSet() && gameDisk == NULL)
        {
            gameDisk = new Disk("Disk", sSceneMgr, gameSimulator, 1.0f/*Ogre::Math::RangeRandom(0,2)*/, 1);
            gameDisk->addToSimulator();
        }    
        
        if (!powerUpsSpawned && gameSimulator->checkSafeToSpawnPowerUps())
            activatePowerUps();
    }

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

            Player *newP = new Player(playerBuffer, sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), numberOfClients, activeRoom, MasterControl->gui);
            newP->setPlayerGameState(PLAYING);
            playerGameStatesList[newP->getPlayerID() - 1] == PLAYING;
            playerList[numberOfClients-1] = newP;
            playerList[numberOfClients-1]->addToSimulator();
        }
    }

    for (int i = 0; i < numberOfClients; i++)  // CHECK FOR ACTIVITY FROM CURRENT PLAYERS
    {
        if (gameNetwork->checkSockets(i))
            interpretClientPacket(i);

        updateClientVelocity(playerList[i]);
        
        // restrictPlayerMovement(playerList[i]);
        endPlayerGame(i);
    }
    
    if (pseudoHostStartGame && (((float)(clock() - updateClock))/CLOCKS_PER_SEC  > 0.01f || forceUpdate))
    {
        constructAndSendGameState();
        updateClock = clock();
        forceUpdate = false;
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
bool Server::constructAndSendGameState()
{
    char* sBuff;
    char* dBuff;
    char* tBuff;
    char* puBuff;
    char* gBuff = NULL;
    char* pgStateBuff;
    int totalBytesSent = 0;

    vector<S_PLAYER_packet> playerPackList; 
    vector<TILE_packet> tilePackList;
    vector<POWERUP_packet> powerUpPackList;
    DISK_packet dPack;
    
    sBuff = new char[sizeof(S_PLAYER_packet)];
    puBuff = new char[sizeof(POWERUP_packet)];
    tBuff = new char[sizeof(TILE_packet)];
    dBuff = new char[sizeof(DISK_packet)];
    pgStateBuff = new char[sizeof(GAMESTATE_packet)];
    gBuff = new char[sizeof(GAMESTATE_packet)];


    /* CREATE LIST OF PLAYER UPDATE PACKETS */
    for (int i = 0; i < numberOfClients; i++)   // might want to limit to players who have changed position
    {
        S_PLAYER_packet sPack;
     
        memset(&sPack, 0, sizeof(S_PLAYER_packet));

        sPack.packetID = (char)(((int)'0') + S_PLAYER);
        sPack.playID = (char)(((int)'0') + (i + 1));

        sPack.x = playerList[i]->getSceneNode()->_getDerivedPosition().x;
        sPack.y = playerList[i]->getSceneNode()->_getDerivedPosition().y;
        sPack.z = playerList[i]->getSceneNode()->_getDerivedPosition().z;
        
        sPack.orientation = playerList[i]->getSceneNode()->_getDerivedOrientation();
        playerPackList.push_back(sPack);
    }
    /* START GAME PACKET */
    if (pseudoHostStartGame && !gameRoomCreated)
    {
        gameRoomCreated = true;

        GAMESTATE_packet gPack;
        
        gPack.packetID = (char)(((int)'0') + GAMESTATE);
        gPack.stateID = (char)(((int)'0') + START);
        gPack.stateAttribute = (char)(((int)'0') + numberOfClients);

        memcpy(gBuff, &gPack, sizeof(DISK_packet));
    }
    /* CREATE LIST OF TILE UPDATE PACKETS */
    if (updateRemovedTiles())
    {
        for (int i = 0; i < removedTiles.size(); i++)
        {
            Tile* localTile = removedTiles[i];

            TILE_packet tPack;

            tPack.packetID = (char)(((int)'0') + TILE);
            tPack.playID = (char)(((int)'0') + (localTile->getTileOwner()));
            tPack.removed = '1';
            tPack.tileNumber = localTile->getTileNumber();

            tilePackList.push_back(tPack);
        }
        removedTiles.clear();
    }
    /* CREATE LIST OF POWER UP UPDATE PACKETS */
    if (powerUpsSpawned && updatePowerUps())
    {
        for (int i = 0; i < removedPowerUps.size(); i++)
        {
            Target* localTarget = removedPowerUps[i];
        
            POWERUP_packet puPack;

            puPack.packetID = (char)(((int)'0') + POWERUP);
            puPack.powerID = (char)(((int)'0') + localTarget->getPowerUpType());
            puPack.receiverID = (char)(((int)'0') + localTarget->getReceiverID()); // 1-4 means apply to player 
            puPack.index = (char)(((int)'0') + localTarget->getIndex());

            puPack.x = localTarget->getSceneNode()->getPosition().x;
            puPack.y = localTarget->getSceneNode()->getPosition().y;
            puPack.z = localTarget->getSceneNode()->getPosition().z;

            powerUpPackList.push_back(puPack);
        }
        removedPowerUps.clear();
    }
    for (int i = 0; i < activePowerUps.size(); i++)
    {
        Target* localTarget = activePowerUps[i];
        
        POWERUP_packet puPack;

        puPack.packetID = (char)(((int)'0') + POWERUP);
        puPack.powerID = (char)(((int)'0') + localTarget->getPowerUpType());
        puPack.receiverID = (char)(((int)'0') + 0); // 0 indicates update position
        puPack.index = (char)(((int)'0') + localTarget->getIndex());

        puPack.x = localTarget->getSceneNode()->getPosition().x;
        puPack.y = localTarget->getSceneNode()->getPosition().y;
        puPack.z = localTarget->getSceneNode()->getPosition().z;

        powerUpPackList.push_back(puPack);
    }
    /* CREATE LIST OF DISK PACKETS */
    if (gameDisk != NULL)   // HARD CODE DISK FLAG
    {
        memset(&dPack, 0, sizeof(DISK_packet));

        dPack.packetID = (char)(((int)'0') + DISK);
        dPack.diskID = (char)((int)'0');
        dPack.playID = (char)(((int)'0') + gameDisk->checkIDOfHolder());

        dPack.x = gameDisk->getSceneNode()->_getDerivedPosition().x;
        dPack.y = gameDisk->getSceneNode()->_getDerivedPosition().y;
        dPack.z = gameDisk->getSceneNode()->_getDerivedPosition().z;

        dPack.orientation = gameDisk->getSceneNode()->_getDerivedOrientation();
    
        memcpy(dBuff, &dPack, sizeof(DISK_packet));
    }

    /* Send all packets to each player */
    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
    {
        if (playerList[i] != NULL)
        {
            playerGameStates localState = playerList[i]->getPlayerGameState();

            if ((localState == WIN || localState == LOSE) && playerGameStatesList[i] == PLAYING)
            {
                GAMESTATE_packet pgsPack;
        
                pgsPack.packetID = (char)(((int)'0') + GAMESTATE);
                pgsPack.stateID = (char)(((int)'0') + PLAYERGAMESTATE);
                pgsPack.stateAttribute = (char)(((int)'0') + localState);

                printf("Server Sending state ID: %c\n", pgsPack.stateID);

                memcpy(pgStateBuff, &pgsPack, sizeof(GAMESTATE_packet));
                gameNetwork->sendPacket(pgStateBuff, i);
                playerGameStatesList[i] = localState;
            }
            /* All Disk Packets */
            if (gameDisk != NULL)
                gameNetwork->sendPacket(dBuff, i);
            /* Send game state packet */
            if (gBuff != NULL)
                gameNetwork->sendPacket(gBuff, i);
            /* All Player packets */
            for (int j = 0; j < playerPackList.size(); j++)
            {
                memcpy(sBuff, &playerPackList[j], sizeof(S_PLAYER_packet));
                gameNetwork->sendPacket(sBuff, i);
            }
            /* All tile packets */
            for (int j = 0; j < tilePackList.size(); j++)
            {
                memcpy(tBuff, &tilePackList[j], sizeof(TILE_packet));
                // printf("Tile Removal Packet: \n");
                // printf("\tTile Number: %d\t Tile Owner ID: %c\n\n", tilePackList[j].tileNumber, tilePackList[j].playID);
                gameNetwork->sendPacket(tBuff, i);
            }
            for (int j = 0; j < powerUpPackList.size(); j++)
            {
                memcpy(puBuff, &powerUpPackList[j], sizeof(POWERUP_packet));
                gameNetwork->sendPacket(puBuff, i);
            }
        }
    }
}
//-------------------------------------------------------------------------------------
bool Server::updateRemovedTiles()
{
    gameSimulator->removeTiles(removedTiles);

    if (removedTiles.size() > 0)
        return true;
    else 
        return false;
}
//-------------------------------------------------------------------------------------
bool Server::updatePowerUps()
{
    int ranPType, indexIntoPowers;
    gameSimulator->removeHitPowerUps(removedPowerUps);
    
    for (int i = 0; i < removedPowerUps.size(); i++)
    {
        activePowerUpTypes[removedPowerUps[i]->getPowerUpType()] -= 1;
        removedPowerUps[i]->setActive(false);
    }
    if (removedPowerUps.size() == 0)
        return false;

    for (int i = 0; i < activePowerUps.size(); i++) // check the active power ups for the one that is now inactive and replace it with a random type
    {
        if (!activePowerUps[i]->checkActive())
        {
            ranPType = rand() % 4;

            if (activePowerUpTypes[ranPType] == 4)
            {
                if (ranPType == 3)
                    ranPType--;
                if (ranPType == 0)
                    ranPType++;
            }

            if (ranPType == 0)  // EXPLOSIVE
            {
                for (int j = 0; j < explosiveList.size(); j++)
                {
                    if (!explosiveList[j]->checkActive())
                    {
                        indexIntoPowers = j;
                        break;
                    }
                }
                explosiveList[indexIntoPowers]->setActive(true);
                explosiveList[indexIntoPowers]->addToSimulator();
                activePowerUps[i] = explosiveList[indexIntoPowers];
            }
            else if (ranPType == 1)  // SPEED
            {
                for (int j = 0; j < speedList.size(); j++)
                {
                    if (!speedList[j]->checkActive())
                    {
                        indexIntoPowers = j;
                        break;
                    }
                }
                speedList[indexIntoPowers]->setActive(true);
                speedList[indexIntoPowers]->addToSimulator();
                activePowerUps[i] = speedList[indexIntoPowers];
            }
            else if (ranPType == 2)  // JUMP
            {
                for (int j = 0; j < jumpList.size(); j++)
                {
                    if (!jumpList[j]->checkActive())
                    {
                        indexIntoPowers = j;
                        break;
                    }
                }
                jumpList[indexIntoPowers]->setActive(true);
                jumpList[indexIntoPowers]->addToSimulator();
                activePowerUps[i] = jumpList[indexIntoPowers];
            }
            else if (ranPType == 3)  // RESTORE
            {
                for (int j = 0; j < restoreList.size(); j++)
                {
                    if (!restoreList[j]->checkActive())
                    {
                        indexIntoPowers = j;
                        break;
                    }
                }
                restoreList[indexIntoPowers]->setActive(true);
                restoreList[indexIntoPowers]->addToSimulator();
                activePowerUps[i] = restoreList[indexIntoPowers];
            }
        }
    }
    if (removedPowerUps.size() > 0)
        return true;
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

    RoomSpace* gp = activeRoom->getPlayerRoomSpace(p->getPlayerID());

    restrictHNode = gp->horizontalGap;
    restrictVNode = gp->verticalGap;

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

    while (indexIntoBuff < MAX_SIZE_OF_BUFFER && buff[indexIntoBuff] != 0x00)
    {
        int packetID = buff[indexIntoBuff] - '0';
        /******************** PLAYER INPUT ********************/
        if (packetID == INPUT)
        {
            INPUT_packet i;
            memcpy(&i, buff+indexIntoBuff, sizeof(INPUT_packet));

            int pID = i.playID - '0';
            processClientInput((pID - 1), i.key);

            indexIntoBuff += sizeof(INPUT_packet);
        }
        /******************** PLAYER ORIENTATION ********************/
        else if (packetID == C_PLAYER)
        {   
            C_PLAYER_packet p;
            
            memcpy(&p, buff+indexIntoBuff, sizeof(C_PLAYER_packet));

            int pID = p.playID - '0';
            Ogre::Quaternion quat = p.orientation;

            Player* cp = playerList[pID-1];

            cp->getSceneNode()->_setDerivedOrientation(p.orientation);

            btQuaternion rotationQ;
            btTransform transform = cp->getBody()->getCenterOfMassTransform();
            rotationQ = btQuaternion(p.orientation.getYaw().valueRadians(), 0, 0);
            transform.setRotation(rotationQ);

            cp->getBody()->setCenterOfMassTransform(transform);

            indexIntoBuff += sizeof(C_PLAYER_packet);
        }
        /******************** SIGHT NODE POSITION ********************/
        else if(packetID == DISK)
        {
            DISK_packet dp;
            memcpy(&dp, buff+indexIntoBuff, sizeof(DISK_packet));

            int dID = dp.diskID - '0';
            int pID = dp.playID - '0';

            Player* cp = playerList[pID-1];
            printf("Updating Sight Node of Player%d\n", pID);

            cp->getSceneNode()->_setDerivedOrientation(dp.orientation);

            btQuaternion rotationQ;
            btTransform transform = cp->getBody()->getCenterOfMassTransform();
            rotationQ = btQuaternion(dp.orientation.getYaw().valueRadians(), 0, 0);
            transform.setRotation(rotationQ);

            cp->getBody()->setCenterOfMassTransform(transform);

            cp->getPlayerSightNode()->setPosition(Ogre::Vector3(dp.x, dp.y, dp.z));

            indexIntoBuff += sizeof(DISK_packet);   
        }
        /******************** GAMESTATE UPDATES ********************/
        else if (packetID == GAMESTATE)
        {
            GAMESTATE_packet g;

            memcpy(&g, buff+indexIntoBuff, sizeof(GAMESTATE_packet));

            if (g.stateID == (char)(((int)'0') + START))
            {
                pseudoHostStartGame = true;

                switchRooms();
            }

            indexIntoBuff += sizeof(GAMESTATE_packet);
        }
    }

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
        case 't':  // PERFORM THROW IN SIMULATOR HANDLES IF THROW FLAG IS SET
            if (playerList[playerIndex]->checkHolding())
            {
                gameSimulator->setThrowFlag();
                forceUpdate = true;
            }
            break;
        case 'q':  // GAME STATE CHANGE
            removePlayer(playerIndex);
            break;
        case 'k':// ENTER: // GAME STATE CHANGE
            break;
    }
}
//-------------------------------------------------------------------------------------
void Server::removePlayer(int playerIndex)
{
    playerList[playerIndex]->setPlayerGameState(DISCONNECTED);
    playerGameStatesList[playerIndex] = DISCONNECTED;
    playerList[playerIndex]->getSceneNode()->setVisible(false);
    gameSimulator->removePlayer(playerIndex);
}
//-------------------------------------------------------------------------------------
void Server::switchRooms()
{
    if (numberOfClients > 2 && twoPlayerGameRoom->checkActive())
    {
        twoPlayerGameRoom->deactivateRoom();
        fourPlayerGameRoom->activateRoom();
        activeRoom = fourPlayerGameRoom;

        for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
        {
            if (playerList[i] != NULL)
            {
                playerList[i]->changeGameRoom(activeRoom);
                playerList[i]->setPlayerStartingPosition(true);
                /* Adjust for any players that might have left */
            }
        }

        if (numberOfClients == 3)
            fourPlayerGameRoom->deactivateRoomSpace(4);
    }
    else if (numberOfClients == 2 && fourPlayerGameRoom->checkActive())
    {
        fourPlayerGameRoom->deactivateRoom();
        twoPlayerGameRoom->activateRoom();
        activeRoom = twoPlayerGameRoom;

        // for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
        // {
        //     if (playerList[i] != NULL)
        //     {
        //         playerList[i]->changeGameRoom(activeRoom);
        //         /* Adjust for any players that might have left */
        //     }
        // }
    }
}
//-------------------------------------------------------------------------------------
void Server::activatePowerUps()
{
    activePowerUpTypes = vector<int>(NUM_OF_POWERUP_TYPES, 0);
    for (int i = 0; i < activePowerUps.size(); i++)
    {
        gameSimulator->removeObject(activePowerUps[i]->getGameObjectName());
        if (i == activePowerUps.size() - 1)
            activePowerUps.clear();
    }
    int ranPType;
    int indexIntoPowers;
    for (int i = 0; i < numberOfClients; i++)
    {
        ranPType = rand() % 4;

        indexIntoPowers = activePowerUpTypes[ranPType];
        activePowerUpTypes[ranPType] += 1;

        if (ranPType == 0)  // EXPLOSIVE
        {
            explosiveList[indexIntoPowers]->setActive(true);
            explosiveList[indexIntoPowers]->addToSimulator();
            activePowerUps.push_back(explosiveList[indexIntoPowers]);
        }
        else if (ranPType == 1)  // SPEED
        {
            speedList[indexIntoPowers]->setActive(true);
            speedList[indexIntoPowers]->addToSimulator();
            activePowerUps.push_back(speedList[indexIntoPowers]);
        }
        else if (ranPType == 2)  // JUMP
        {
            jumpList[indexIntoPowers]->setActive(true);
            jumpList[indexIntoPowers]->addToSimulator();
            activePowerUps.push_back(jumpList[indexIntoPowers]);
        }
        else if (ranPType == 3)  // RESTORE
        {
            restoreList[indexIntoPowers]->setActive(true);
            restoreList[indexIntoPowers]->addToSimulator();
            activePowerUps.push_back(restoreList[indexIntoPowers]);
        }
    }
    powerUpsSpawned = true;
}
//-------------------------------------------------------------------------------------
void Server::endPlayerGame(int pIndex)
{
    if (playerList[pIndex] != NULL && playerList[pIndex]->getPlayerGameState() == PLAYING &&
            playerList[pIndex]->getSceneNode()->getPosition().y < activeRoom->getFloorPositionY() - 15.0f)
    {
        gameSimulator->removeObject(playerList[pIndex]->getGameObjectName());
        playerList[pIndex]->getSceneNode()->setVisible(false);
        playerList[pIndex]->setPlayerGameState(LOSE);
    }
}
//-------------------------------------------------------------------------------------
void Server::restartRound()
{

}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
