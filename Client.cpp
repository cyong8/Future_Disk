#include "Client.h"

Client::Client(char* IP, Ogre::SceneManager* mgr) // created in MCP
{	// will most likely pass a GUI object on creation as well
	cSceneMgr = mgr;
	clientOrientationChange = false;
    mRotate = 0.1f;
    gameStart = false;
    gameDisk = NULL;
    numPlayers = 0;

    playerList = vector<Player*>(MAX_NUMBER_OF_PLAYERS, NULL);

    gameNetwork = new Network(CLIENT, IP);
    gameMusic = new Music();

    printf("\n\n\nIP: %s\n\n\n", IP);
    playerID = gameNetwork->establishConnection();  
    
    createScene();
}
//-------------------------------------------------------------------------------------
Client::~Client() 
{
}
//-------------------------------------------------------------------------------------
void Client::createScene()
{   // Don't create the players scene until it has been given a player ID by the Server
	char playerBuffer[25];
    char cameraBuffer[25];

    /* GAME ROOM */
    twoPlayerGameRoom = new Room(cSceneMgr, NULL, 2);
    fourPlayerGameRoom = new Room(cSceneMgr, NULL, 4);

    cSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    cSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create Light for room
    directLight = cSceneMgr->createLight("roomLight");  // Point light
    directLight->setType(Ogre::Light::LT_POINT);
    directLight->setDiffuseColour(Ogre::ColourValue::White);
    directLight->setSpecularColour(Ogre::ColourValue::White);
    directLight->setVisible(true);

    if (playerID > 2)
    {
        fourPlayerGameRoom->activateRoom();
        activeRoom = fourPlayerGameRoom;
    }
    else
    {
        twoPlayerGameRoom->activateRoom();
        activeRoom = twoPlayerGameRoom;
    }
    /********************  POWER UPS  ********************/
    Target* Power;
    Target* Speed;
    Target* JumpPower;
    Target* Restore;
    for (int i = 1; i <= MAX_NUMBER_OF_PLAYERS; i++)
    {
        Power = new Target("Explosive_" + Ogre::StringConverter::toString(i), cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, EXPLOSIVE, i-1);
        explosiveList.push_back(Power);
        
        Speed = new Target("Speed_" + Ogre::StringConverter::toString(i), cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, SPEED, i-1);
        speedList.push_back(Speed);
        
        JumpPower = new Target("Jump_" + Ogre::StringConverter::toString(i), cSceneMgr,  NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, JUMPBOOST, i-1);
        jumpList.push_back(JumpPower);
        
        Restore = new Target("Restore_" + Ogre::StringConverter::toString(i), cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), activeRoom, RESTORE, i-1);
        restoreList.push_back(Restore);
    }

    printf("\n\nPlayer ID: %d\n\n", playerID);

    /* CLIENT PLAYER */
    sprintf(playerBuffer, "Player%d", playerID);
    clientPlayer = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), playerID, activeRoom);
    clientPlayer->setPlayerSpace();

    // for (int i = 0; i < clientPlayer->getPlayerSpace()->tileList.size(); i++)
    // {
    //     Tile* ct = clientPlayer->getPlayerSpace()->tileList[i];
    //     printf("Player: %d\t Tile #: %d\n", ct->getTileOwner(), ct->getTileNumber());
    // }

    playerList[playerID-1] = clientPlayer;
    numPlayers++;

    /* CLIENT'S PLAYER CAMERA */
	sprintf(cameraBuffer, "Player%dCam", playerID);
    pCam = new PlayerCamera(cameraBuffer, cSceneMgr, cSceneMgr->getCamera("PlayerCam"));
    pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(clientPlayer);

    createOverlays(pCam);
    updateClock = clock();
    previousPosition = clientPlayer->getSceneNode()->getPosition();
}
//-------------------------------------------------------------------------------------
bool Client::frameRenderingQueued(Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    if (gameNetwork->checkSockets(0))
        updateScene();

    updateCamera(); 

    if(clientPlayer->getCustomAnimationState() != NULL)
        clientPlayer->getCustomAnimationState()->addTime(tSinceLastFrame);

    if(gameDisk != NULL && gameDisk->diskAnimationState != NULL)
        gameDisk->diskAnimationState->addTime(tSinceLastFrame*2);
   
    processUnbufferedInput(mKeyboard, mMouse);

    if(clientPlayer->checkState(HOLDING))
    {
        if(!clientPlayer->catchAnimation)
            clientPlayer->animateCharacter("catch");
        
        clientPlayer->catchAnimation = true;
    }
    else
    {
        clientPlayer->catchAnimation = false;
    }
}
//-------------------------------------------------------------------------------------
void Client::processUnbufferedInput(OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    if (!gameStart && (mKeyboard->isKeyDown(OIS::KC_RETURN) || mKeyboard->isKeyDown(OIS::KC_NUMPADENTER)) && playerID == 1) //&& numPlayers >= 2)
    {
        char* gBuff = new char[sizeof(GAMESTATE_packet)];
        
        GAMESTATE_packet gPack;
        gPack.packetID = (char)(((int)'0') + GAMESTATE);
        gPack.stateID = (char)(((int)'0') + START);
        gPack.stateAttribute = ' ';

        memcpy(gBuff, &gPack, sizeof(GAMESTATE_packet));
        gameNetwork->sendPacket(gBuff, playerID);
    }
    else if (!gameStart)
        return;

    char* cpBuff;
    static bool vKeydown = false;
    INPUT_packet pack;
    char* iBuff = new char[sizeof(INPUT_packet)];

    pack.packetID = (char)(((int)'0') + INPUT);
    pack.playID = (char)(((int)'0') + playerID);

    if (clientOrientationChange && ((float)(clock() - updateClock))/CLOCKS_PER_SEC  > 0.016f) 
    {
        clientOrientationChange = false;
       
        C_PLAYER_packet cPack;
        cPack.packetID =(char)(((int)'0') + C_PLAYER);
        cPack.playID = (char)(((int)'0') + playerID);
        cPack.orientation = clientPlayer->getSceneNode()->_getDerivedOrientation();
        // printf("Client Sending Quaternion: %f, %f, %f\n", pack.orientation.getYaw().valueRadians(), pack.orientation.getRoll().valueRadians(), pack.orientation.getPitch().valueRadians());
       
        cpBuff = new char[sizeof(C_PLAYER_packet)];
        memcpy(cpBuff, &cPack, sizeof(C_PLAYER_packet));

        gameNetwork->sendPacket(cpBuff, playerID);
        updateClock = clock();
    }
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        /* Close player socket and allow for another player to take its place */
        pack.key = 'q';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
        return;
    }
    /*WALKING ANIMATION*/
    if (!mKeyboard->isKeyDown(OIS::KC_W) && !mKeyboard->isKeyDown(OIS::KC_A) 
    && !mKeyboard->isKeyDown(OIS::KC_S) && !mKeyboard->isKeyDown(OIS::KC_D)) 
    {
        if(clientPlayer->moving)
            clientPlayer->nullAnimationState();

        clientPlayer->moving = false;
    }
    else
    {
        if(!clientPlayer->moving)
            clientPlayer->animateCharacter("walk");

        clientPlayer->moving = true;
    }
    /* MOVE FORWARD */
    if (mKeyboard->isKeyDown(OIS::KC_W) && !clientPlayer->checkState(FORWARD))
    {
        pack.key = 'w';

        clientPlayer->setState(FORWARD, true);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_W) && clientPlayer->checkState(FORWARD))
    {
        pack.key = 'w';

        clientPlayer->setState(FORWARD, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE LEFT */
    if (mKeyboard->isKeyDown(OIS::KC_A) && !clientPlayer->checkState(LEFT))
    {
        pack.key = 'a';

        clientPlayer->setState(LEFT, true);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_A) && clientPlayer->checkState(LEFT))
    {
        pack.key = 'a';

        clientPlayer->setState(LEFT, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE BACK */
    if (mKeyboard->isKeyDown(OIS::KC_S) && !clientPlayer->checkState(BACK))
    {
        pack.key = 's';

        clientPlayer->setState(BACK, true);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_S) && clientPlayer->checkState(BACK))
    {
        pack.key = 's';

        clientPlayer->setState(BACK, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE RIGHT */
    if (mKeyboard->isKeyDown(OIS::KC_D) && !clientPlayer->checkState(RIGHT))
    {
        pack.key = 'd';

        clientPlayer->setState(RIGHT, true);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_D) && clientPlayer->checkState(RIGHT))
    {
        pack.key = 'd';

        clientPlayer->setState(RIGHT, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_SPACE)) //&& !clientPlayer->checkState(JUMP))     // SET BACK TO FALSE W/ GAMESTATE PACKET (when player hits ground) 
    {
        clientPlayer->animateCharacter("jump");
        pack.key = 'j';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_LSHIFT) && !clientPlayer->checkState(BOOST))
    {
        pack.key = 'b';

        clientPlayer->setState(BOOST, true);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) && clientPlayer->checkState(BOOST))
    {
        pack.key = 'b';

        clientPlayer->setState(BOOST, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_V) && !clientPlayer->checkState(VIEWMODE))            // Aim View Toggle - Send to Server so they can let you throw; update camera position on client end
    {
        pCam->toggleThirdPersonView();
        clientPlayer->setState(VIEWMODE, true);
        
        if (gameDisk != NULL)
            gameDisk->getSceneNode()->setVisible(false);
        pCam->initializePosition(clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_V) && clientPlayer->checkState(VIEWMODE))        
    {
        pCam->toggleThirdPersonView();
        clientPlayer->setState(VIEWMODE, false);

        if (gameDisk != NULL)
            gameDisk->getSceneNode()->setVisible(true);
        pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
    if (mMouse->getMouseState().buttonDown(OIS::MB_Left) && clientPlayer->checkState(VIEWMODE) && clientPlayer->checkState(HOLDING))
    {
        /* Using Disk packet to send position of player's sight node (i.e. Direction of throw) */
        clientPlayer->animateCharacter("throw");

        DISK_packet dPack;
        char* dBuff = new char[sizeof(DISK_packet)];

        dPack.packetID =(char)(((int)'0') + DISK);
        dPack.diskID = (char)(((int)'0') + playerID);
        dPack.playID = (char)(((int)'0') + playerID);

        // Grab Sight Node position
        Ogre::Vector3 sightPos = clientPlayer->getPlayerSightNode()->getPosition();

        dPack.x = sightPos.x;
        dPack.y = sightPos.y;
        dPack.z = sightPos.z;
        dPack.orientation = clientPlayer->getSceneNode()->_getDerivedOrientation();

        memcpy(dBuff, &dPack, sizeof(DISK_packet));
        gameNetwork->sendPacket(dBuff, playerID);
        
        /* Send the input packet after the direction, so that throw isn't done before updated direction */
        pack.key = 't';

        clientPlayer->setHolding(false);
        clientPlayer->setState(HOLDING, false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);

        gameDisk->getSceneNode()->setVisible(true);
    }
}
//-------------------------------------------------------------------------------------
void Client::updateScene() // Receive packets and interpret them
{
	char* packList = gameNetwork->receivePacket(playerID);

    interpretServerPacket(packList);
}
//-------------------------------------------------------------------------------------
void Client::updateCamera()
{
    Ogre::SceneNode* pSightNode = clientPlayer->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = clientPlayer->getPlayerCameraNode();

    Ogre::Vector3 positionDiffVector = clientChangePosition();

    if (positionDiffVector != Ogre::Vector3::ZERO)
    {
        pSightNode->_setDerivedPosition(pSightNode->_getDerivedPosition() + positionDiffVector);
        pCamNode->_setDerivedPosition(pCamNode->_getDerivedPosition() + positionDiffVector);
    }

    if (pCam->isInAimMode())
        pCam->update(clientPlayer->getSceneNode()->_getDerivedPosition(), pSightNode->_getDerivedPosition());
    else
        pCam->update(pCamNode->_getDerivedPosition(), pSightNode->_getDerivedPosition());
}
//-------------------------------------------------------------------------------------
void Client::interpretServerPacket(char* packList)
{
    int indexIntoBuff = 0;
    int packetsFound = 0;
    int newPlayerID, playerIndex;
    
    while (indexIntoBuff < MAX_SIZE_OF_BUFFER && packList[indexIntoBuff] != 0x00)
    {
        packetsFound++;
        char packType = packList[indexIntoBuff];
        // printf("PACKET FOUND %d!\n", packetsFound);

        /* UPDATE DISK */
        if (packType == (char)(((int)'0') + DISK))
        {
            // printf("updating disk\n");
            DISK_packet d;
            memcpy(&d, packList+indexIntoBuff, sizeof(DISK_packet));

            if (gameDisk == NULL)
            {
                gameDisk = new Disk("Disk", cSceneMgr, NULL, -1.0f, 1);
                gameDisk->particleNode->setVisible(true);
                gameDisk->diskAnimationState = gameDisk->diskEnt->getAnimationState("spin");
                gameDisk->diskAnimationState->setEnabled(true);
                gameDisk->diskAnimationState->setLoop(true);
                gameDisk->diskAnimationState->setTimePosition(0);
            }
            if (d.playID == (char)(((int)'0') + playerID))
            {
                clientPlayer->setState(HOLDING, true);
                gameStart = true;
            }

            gameDisk->getSceneNode()->_setDerivedPosition(Ogre::Vector3(d.x, d.y, d.z));
            gameDisk->getSceneNode()->_setDerivedOrientation(d.orientation);

            indexIntoBuff += sizeof(DISK_packet);
        }
        /* UPDATE PLAYERS */
        else if (packType == (char)(((int)'0') + S_PLAYER))
        {
            S_PLAYER_packet p;
            memcpy(&p, packList+indexIntoBuff, sizeof(S_PLAYER_packet));

            newPlayerID = p.playID - '0';
            playerIndex = newPlayerID - 1;
            // printf("*******New player ID: %d\n\n", newPlayerID);
            /* NEW PLAYER NOT BEING ADDED TO SCENE */
            if (playerList[playerIndex] == NULL)
            {
                // printf("*****ADDING NEW PLAYER\n\n");
                char playerBuffer[25];
                sprintf(playerBuffer, "Player%d", newPlayerID);

                playerList[playerIndex] = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), newPlayerID, activeRoom);
                playerList[playerIndex]->setPlayerSpace();
                numPlayers++;
            }
            if (newPlayerID != playerID)
            {
                playerList[playerIndex]->getSceneNode()->_setDerivedPosition(Ogre::Vector3(p.x, p.y, p.z));
                playerList[playerIndex]->getSceneNode()->_setDerivedOrientation(p.orientation);
            }
            else
                clientPlayer->getSceneNode()->_setDerivedPosition(Ogre::Vector3(p.x, p.y, p.z));

            indexIntoBuff += sizeof(S_PLAYER_packet);
        }
        /* UPDATE TILES */
        else if (packType == (char)(((int)'0') + TILE))
        {
            TILE_packet t;
            memcpy(&t, packList+indexIntoBuff, sizeof(TILE_packet));

            newPlayerID = (t.playID - '0'); 
            Tile* localTile = playerList[newPlayerID - 1]->getPlayerSpace()->tileList[t.tileNumber];

            if (t.removed == (char)(((int)'0') + 1))
            {
                localTile->getSceneNode()->setVisible(false);
                gameDisk->resetPowerUp();
            }
            // printf("Tile Removal Packet: \n");
            // printf("\tTile Number: %d\t Tile Owner ID: %d\n\n", t.tileNumber, newPlayerID);
            indexIntoBuff += sizeof(TILE_packet);
        }
        /* UPDATE POWERUP */
        else if (packType == (char)(((int)'0') + POWERUP))
        {
            POWERUP_packet pu;
            memcpy(&pu, packList+indexIntoBuff, sizeof(POWERUP_packet));

            int typeOfPowerUp = (pu.powerID - '0');
            int powerUpIndex = (pu.index - '0');
            int receiver = (pu.receiverID - '0');

            Target* localTarget;

            if (typeOfPowerUp == EXPLOSIVE)
                localTarget = explosiveList[powerUpIndex];
            else if (typeOfPowerUp == SPEED)
                localTarget = speedList[powerUpIndex];
            else if (typeOfPowerUp == JUMPBOOST)
                localTarget = jumpList[powerUpIndex];
            else if (typeOfPowerUp == RESTORE)
                localTarget = restoreList[powerUpIndex];
           
           if (receiver == 0)                       // updating a target's position
           {
                if (!localTarget->checkActive())
                {
                    localTarget->setActive(true);
                    // localTarget->getSceneNode()->setVisible(true);
                }
                localTarget->getSceneNode()->setPosition(pu.x, pu.y, pu.z);
            }
            else
            {
                localTarget->setActive(false);
                // localTarget->getSceneNode()->setVisible(false);
                
                if (typeOfPowerUp == EXPLOSIVE || typeOfPowerUp == SPEED)
                {
                    if (receiver <= 4)
                    {
                        // change so that receiver indexes into array of disks
                        gameDisk->activatePowerUp(localTarget->getPowerUpType(), NULL);
                    }
                }
                else if (typeOfPowerUp == JUMPBOOST)
                    playerList[receiver - 1]->increaseJump();
            }

            indexIntoBuff += sizeof(POWERUP_packet);
        }
        else if(packType == (char)(((int)'0') + GAMESTATE))
        {
            GAMESTATE_packet g;
            memcpy(&g, packList+indexIntoBuff, sizeof(GAMESTATE_packet));

            if (g.stateID == (char)(((int)'0') + START))
            {
                int playersInRoom = (g.stateAttribute - '0'); 
                
                switchRooms(playersInRoom);
            }
            else if (g.stateID == (char)(((int)'0') + QUIT))
            {

            }
            else if (g.stateID == (char)(((int)'0') + SOUND))
            {

            }
            else if (g.stateID == (char)(((int)'0') + ENDROUND))
            {

            }
            indexIntoBuff += sizeof(GAMESTATE_packet);
        }
    }
    // printf("ENDING INTERPRETING PACKETS\n\n\n");
}
//-------------------------------------------------------------------------------------
bool Client::mouseMoved(Ogre::Real relX, Ogre::Real relY)
{
    if (!gameStart)
        return false;

    Ogre::SceneNode* pSceneNode = clientPlayer->getSceneNode();
    Ogre::SceneNode* pSightNode = clientPlayer->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = clientPlayer->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;
    /* NEED TO UPDATE SIGHT AND CAMERA NODES HERE INSTEAD OF RELYING ON RENDER LOOP TO DO SO - TOO LATE THEN */
    if (pCam->isInAimMode())
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate /2) * relX), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -relY, 0.0f);
       	clientOrientationChange = true;
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * relX), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -relY, 0.0f);
        clientOrientationChange = true;
    }

    pSightNode->setPosition(pSightNode->getPosition() + sightHeight);
    pCamNode->setPosition(Ogre::Vector3(pCamNode->getPosition().x, pCamNode->getPosition().y, -pSightNode->getPosition().z));
    updateCamera();

    return true;
}
//-------------------------------------------------------------------------------------
void Client::createOverlays(PlayerCamera* playCam) // might move to Client and Server
{
    /********************    MENUS    ********************/
    Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
    
    Ogre::Overlay* crossHairVertOverlay = overlayManager->create("crossHairVert"); // Create an overlay for the vertical crosshair

    // Create an overlay container for the vertical crosshair
    Ogre::OverlayContainer* crossHairVertContainer = static_cast<Ogre::OverlayContainer*>( overlayManager->createOverlayElement("Panel", "VerticalPanel"));
    crossHairVertContainer->setPosition(0.5f, 0.4f);
    crossHairVertContainer->setDimensions(0.001f, 0.2f);
    crossHairVertContainer->setMaterialName("BaseWhite");
    crossHairVertContainer->getMaterial()->setReceiveShadows(false);

    crossHairVertOverlay->add2D( crossHairVertContainer ); // Add crossHairVertContainer to the crossHairVertOverlay

    Ogre::Overlay* crossHairHorizOverlay = overlayManager->create("crossHairHoriz"); // Create an overlay for the horizontal crosshair

    // Create an overlay container for the horizontal crosshair
    Ogre::OverlayContainer* crossHairHorizContainer = static_cast<Ogre::OverlayContainer*>(overlayManager->createOverlayElement("Panel", "HorizontalPanel"));
    crossHairHorizContainer->setPosition(0.425, 0.5);
    crossHairHorizContainer->setDimensions(0.15, 0.001);
    crossHairHorizContainer->setMaterialName("BaseWhite");
    crossHairHorizContainer->getMaterial()->setReceiveShadows(false);

    crossHairHorizOverlay->add2D(crossHairHorizContainer);     // Add the crossHairHorizContainer to the crossHairHorizOverlay

    crossHairVertOverlay->hide();    // Hide the Crosshair till 
    crossHairHorizOverlay->hide();   // til Aim View is activated 

    playCam->setCHOverlays(crossHairVertOverlay, crossHairHorizOverlay);
}
//-------------------------------------------------------------------------------------
Ogre::Vector3 Client::clientChangePosition()
{
    Ogre::Vector3 currentPosition = clientPlayer->getSceneNode()->getPosition();
    Ogre::Vector3 diffVector = currentPosition - previousPosition;

    if (previousPosition !=  currentPosition)
    {
        previousPosition = currentPosition;
        return diffVector;
    }
    else 
        return diffVector;
}
//-------------------------------------------------------------------------------------
void Client::switchRooms(int playersInRoom)
{
    if (playersInRoom > 2)
    {
        if (!fourPlayerGameRoom->checkActive()) // Not currently in 4Player Room - Switch
        {
            twoPlayerGameRoom->deactivateRoom();
            fourPlayerGameRoom->activateRoom();
            activeRoom = fourPlayerGameRoom;

            for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
            {
                if (playerList[i] != NULL)
                {
                    playerList[i]->changeGameRoom(activeRoom);
                    /* Adjust for any players that might have left
                        (i.e. player 4 becomes player 2 if 2 and 3 left)
                        need to do this whether or not the room shifted****
                    */
                }
            }

            if (playersInRoom == 3)
                fourPlayerGameRoom->deactivateRoomSpace(4);
        }
    }
    else 
    {
        if (!twoPlayerGameRoom->checkActive()) // Not currently in 2Player Room - Switch
        {
            fourPlayerGameRoom->deactivateRoom();
            twoPlayerGameRoom->activateRoom();
            activeRoom = twoPlayerGameRoom;
            /* Still need to adjust client players and client player positions */
            // for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
            // {
            //     if (playerList[i] != NULL)
            //     {
            //         playerList[i]->changeGameRoom();
            //         /* Adjust for any players that might have left */
            //     }
            // }
        }
    }
}
//-------------------------------------------------------------------------------------
