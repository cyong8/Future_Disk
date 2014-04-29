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
    gameRoom = new Room(cSceneMgr, NULL, 1, 2);
    printf("\n\nPlayer ID: %d\n\n", playerID);

    /* CLIENT PLAYER */
    sprintf(playerBuffer, "Player%d", playerID);
    clientPlayer = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), playerID, Ogre::Vector3(gameRoom->getWidth(), gameRoom->getHeight(), (Ogre::Real)gameRoom->getNumberOfPlayers()));
    playerList[playerID-1] = clientPlayer;
    numPlayers++;

    /* CLIENT'S PLAYER CAMERA */
	sprintf(cameraBuffer, "Player%dCam", playerID);
    pCam = new PlayerCamera(cameraBuffer, cSceneMgr, cSceneMgr->getCamera("PlayerCam"));
    pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(clientPlayer);
    
    /********************  POWER UPS  ********************/
    // Power = new Target("Power", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f)); // Create initial Power-up
    // Speed = new Target("Speed", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f)); // Create initial Power-up
    // JumpPower = new Target("Jump", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f)); // Create initial Power-up
    // Restore = new Target("Restore", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f)); // Create initial Power-up

    cSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    cSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create Light for room
    directLight = cSceneMgr->createLight("directionalLight");  // Point light
    directLight->setType(Ogre::Light::LT_POINT);
    directLight->setDiffuseColour(Ogre::ColourValue::White);
    directLight->setSpecularColour(Ogre::ColourValue::White);
    directLight->setVisible(true);
    directLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getHeight()/2, 0.0f));
    // directLight->setDirection(Ogre::Vector3( 1, -1, -1));

    // createOverlays(pCam);
    updateClock = clock();
    previousPosition = clientPlayer->getSceneNode()->getPosition();
}
//-------------------------------------------------------------------------------------
bool Client::frameRenderingQueued(Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    if (gameNetwork->checkSockets(0))
        updateScene();

    updateCamera(); 
   
    processUnbufferedInput(mKeyboard, mMouse);
}
//-------------------------------------------------------------------------------------
void Client::processUnbufferedInput(OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    char* cpBuff;
    static bool vKeydown = false;
    INPUT_packet pack;
    char* iBuff = (char*)malloc(sizeof(INPUT_packet));

    if (clientOrientationChange && ((float)(clock() - updateClock))/CLOCKS_PER_SEC  > 0.016f) 
    {
        clientOrientationChange = false;
       
        C_PLAYER_packet pack;
        pack.packetID =(char)(((int)'0') + C_PLAYER);
        pack.playID = (char)(((int)'0') + playerID);
        pack.orientation = clientPlayer->getSceneNode()->_getDerivedOrientation();
        // printf("Client Sending Quaternion: %f, %f, %f\n", pack.orientation.getYaw().valueRadians(), pack.orientation.getRoll().valueRadians(), pack.orientation.getPitch().valueRadians());
       
        cpBuff = (char*)malloc(sizeof(C_PLAYER_packet));
        memcpy(cpBuff, &pack, sizeof(C_PLAYER_packet));

        gameNetwork->sendPacket(cpBuff, playerID);
        updateClock = clock();
    }
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        /* Close player socket and allow for another player to take its place */
    }
    /* MOVE FORWARD */
    if (mKeyboard->isKeyDown(OIS::KC_W) && !clientPlayer->checkState(FORWARD))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(FORWARD, true);
        pack.key = 'w';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_W) && clientPlayer->checkState(FORWARD))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(FORWARD, false);
        pack.key = 'w';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE LEFT */
    if (mKeyboard->isKeyDown(OIS::KC_A) && !clientPlayer->checkState(LEFT))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(LEFT, true);
        pack.key = 'a';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_A) && clientPlayer->checkState(LEFT))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(LEFT, false);
        pack.key = 'a';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE BACK */
    if (mKeyboard->isKeyDown(OIS::KC_S) && !clientPlayer->checkState(BACK))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(BACK, true);
        pack.key = 's';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_S) && clientPlayer->checkState(BACK))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(BACK, false);
        pack.key = 's';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    /* MOVE RIGHT */
    if (mKeyboard->isKeyDown(OIS::KC_D) && !clientPlayer->checkState(RIGHT))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(RIGHT, true);
        pack.key = 'd';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_D) && clientPlayer->checkState(RIGHT))
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(RIGHT, false);
        pack.key = 'd';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_SPACE)) //&& !clientPlayer->checkState(JUMP))     // SET BACK TO FALSE W/ GAMESTATE PACKET (when player hits ground) 
    {
        pack.packetID =(char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        //clientPlayer->setState(JUMP, true);
        pack.key = 'j';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_LSHIFT) && !clientPlayer->checkState(BOOST))
    {
        pack.packetID = (char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(BOOST, true);
        pack.key = 'b';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) && clientPlayer->checkState(BOOST))
    {
        pack.packetID = (char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        clientPlayer->setState(BOOST, false);
        pack.key = 'b';

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
    }
    if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeydown)            // Aim View Toggle - Send to Server so they can let you throw; update camera position on client end
    {
        pCam->toggleThirdPersonView();
        pack.key = 'v';

        vKeydown = true;
        
        if (gameDisk != NULL)
            gameDisk->getSceneNode()->setVisible(false);
        pCam->initializePosition(clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeydown)        
    {
        pCam->toggleThirdPersonView();
        pack.key = 'v';

        vKeydown = false;

        if (gameDisk != NULL)
            gameDisk->getSceneNode()->setVisible(true);
        pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
    if (mMouse->getMouseState().buttonDown(OIS::MB_Left) && vKeydown && clientPlayer->checkHolding())
    {
        pack.packetID = (char)(((int)'0') + INPUT);
        pack.playID = (char)(((int)'0') + playerID);

        pack.key = 't';
        clientPlayer->setHolding(false);

        memcpy(iBuff, &pack, sizeof(INPUT_packet));
        gameNetwork->sendPacket(iBuff, playerID);
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
            memcpy(&d, packList + indexIntoBuff, sizeof(DISK_packet));

            if (gameDisk == NULL)
            {
                gameDisk = new Disk("Disk", cSceneMgr, NULL, -1.0f);
                gameDisk->particleNode->setVisible(true);
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
            printf("*******New player ID: %d\n\n", newPlayerID);
            /* NEW PLAYER NOT BEING ADDED TO SCENE */
            if (playerList[playerIndex] == NULL)
            {
                // printf("*****ADDING NEW PLAYER\n\n");
                char playerBuffer[25];
                sprintf(playerBuffer, "Player%d", newPlayerID);

                playerList[playerIndex] = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), newPlayerID, Ogre::Vector3(gameRoom->getWidth(), gameRoom->getHeight(), (Ogre::Real)gameRoom->getNumberOfPlayers() + 1));
                numPlayers++;
            }
            if (newPlayerID != playerID)
            {
                // printf("\n\n\n\n\n\nCHANGING PLAYER ORIENTATION\n\n\n\n\n");
                playerList[playerIndex]->getSceneNode()->_setDerivedPosition(Ogre::Vector3(p.x, p.y, p.z));
                playerList[playerIndex]->getSceneNode()->_setDerivedOrientation(p.orientation);
            }
            else
                clientPlayer->getSceneNode()->_setDerivedPosition(Ogre::Vector3(p.x, p.y, p.z));

            indexIntoBuff += sizeof(S_PLAYER_packet);
        }
        // if(packType == 'D')
        // {
        //     clientPlayer->setHolding(true);
        // }

        // if(packType == 'P')
        // {
        //     Power->getSceneNode()->_setDerivedPosition(newPos);
        //     Power->getSceneNode()->needUpdate();
        // }
        // if(packType == 'S')
        // {
        //     Speed->getSceneNode()->_setDerivedPosition(newPos);
        //     Speed->getSceneNode()->needUpdate();
        // }
        // if(packType == 'J')
        // {
        //     JumpPower->getSceneNode()->_setDerivedPosition(newPos);
        //     JumpPower->getSceneNode()->needUpdate();
        // }
        // if(packType == 'R')
        // {
        //     Restore->getSceneNode()->_setDerivedPosition(newPos);
        //     Restore->getSceneNode()->needUpdate();
        // }
        // if (packType == 'H')
        // {
        //     gameRoom->hTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
        // }
        // if (packType == 'C')
        // {
        //     gameRoom->cTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
        // }
        /* BEGIN GAME */ 
        // else if(packType == (char)(((int)'0') + GAMESTATE))
        // {
        //     gameStart = true;
        // }
    }
    // printf("ENDING INTERPRETING PACKETS\n\n\n");
}
//-------------------------------------------------------------------------------------
bool Client::mouseMoved(Ogre::Real relX, Ogre::Real relY)
{
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
Player* Client::getPlayer()
{
    return clientPlayer;
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