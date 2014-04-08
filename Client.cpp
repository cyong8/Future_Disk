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

    for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
        playerList[i] = NULL;

	gameNetwork = new Network(CLIENT, IP);
	gameMusic = new Music();

    playerID = gameNetwork->establishConnection();  
    
    createScene();
    printf("\n\n\nCLIENT %d SCENE CREATED! \n\n\n", playerID);
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

    gameRoom = new Room(cSceneMgr, NULL, CLIENT);
    printf("\n\nPlayer ID: %d\n\n", playerID);

    sprintf(playerBuffer, "Player%d", playerID);
    clientPlayer = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), playerID);
    numPlayers++;
    playerList[playerID] = clientPlayer;

	sprintf(cameraBuffer, "Player%dCam", playerID);
    pCam = new PlayerCamera(cameraBuffer, cSceneMgr, cSceneMgr->getCamera("PlayerCam"));/*need camera object*/
    
    pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(clientPlayer);
    
    Power = new Target("Power", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    Speed = new Target("Speed", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    JumpPower = new Target("Jump", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    Restore = new Target("Restore", cSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up

    cSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    cSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create Light for room
    pointLight = cSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getHeight()/2, 0.0f));

    // createOverlays(pCam);
}
//-------------------------------------------------------------------------------------
bool Client::frameRenderingQueued(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    if (gameNetwork->checkSockets())
        updateScene();

    if (!processUnbufferedInput(evt, mKeyboard, mMouse))
        exit(2);

    updateCamera(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------------
bool Client::processUnbufferedInput(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
	vector<MCP_Packet> packList;
    MCP_Packet pack;
    bool result = false;
    static bool vKeydown = false;
    memset(&pack, 0, sizeof(MCP_Packet));

    // if (clientOrientationChange)
    // {
    //     clientOrientationChange = false;
    //     pack.id = 'o';
    //     pack.orientationQ = clientPlayer->getSceneNode()->_getDerivedOrientation();
    //     packList.push_back(pack);
    //     pack.id = 'S';
    //     pack.x_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().x;
    //     pack.y_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().y;
    //     pack.z_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().z;
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    // {
    //     return false;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_W) && !clientPlayer->checkState(Forward))                              // Forward - implemented
    // {
    //     clientPlayer->setState(Forward, true);
    //     pack.id = 'w';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_A) && !clientPlayer->checkState(Left))                                 // Left - implemented
    // {
    //     clientPlayer->setState(Left, true);
    //     pack.id = 'a';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_S) && !clientPlayer->checkState(Back))                                 // Backwards - implemented
    // {
    //     clientPlayer->setState(Back, true);
    //     pack.id = 's';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_D) && !clientPlayer->checkState(Right))                                 // Right - implemented
    // {
    //     clientPlayer->setState(Right, true);
    //     pack.id = 'd';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !clientPlayer->checkState(Jump))   // Jump - implemented
    // {
    //     clientPlayer->setState(Jump, true);
    //     pack.id = 'j';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeydown)            // Aim View Toggle - Send to Server so they can let you throw; update camera position on client end
    // {
    //     pCam->toggleThirdPersonView();
    //     pack.id = 'v';
    //     packList.push_back(pack);
    //     vKeydown = true;
    //     result = true;
    //     gameDisk->getSceneNode()->setVisible(false);
    //     pCam->initializePosition(clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    // }
    // if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeydown)        
    // {
    //     pCam->toggleThirdPersonView();
    //     pack.id = 'v';
    //     packList.push_back(pack);
    //     vKeydown = false;
    //     result = true;
    //     gameDisk->getSceneNode()->setVisible(true);
    //     pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    // }    
    // if (mKeyboard->isKeyDown(OIS::KC_LSHIFT) && !clientPlayer->checkState(Boost))                          // Speed Boost
    // {
    //     clientPlayer->setState(Boost, true); 
    //     pack.id = 'b';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (mMouse->getMouseState().buttonDown(OIS::MB_Left) && vKeydown && clientPlayer->checkHolding())
    // {
    //     pack.id = 't';
    //     packList.push_back(pack);
    //     result = true;
    //     clientPlayer->setHolding(false);
    // }
    // if (resetMovementState(evt, mKeyboard, packList) || result)
    // {
    //     pack.id = 'n';
    //     packList.push_back(pack);
    //     gameNetwork->sendPacket(packList, 0);
    // }

    return true;
}
//-------------------------------------------------------------------------------------
bool Client::resetMovementState(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, vector<MCP_Packet> &packList)
{
    MCP_Packet pack;
    bool result = false;
    memset(&pack, 0, sizeof(MCP_Packet));

    // if (!mKeyboard->isKeyDown(OIS::KC_W) && clientPlayer->checkState(Forward))
    // {
    //     clientPlayer->setState(Forward, false);
    //     pack.id = 'w';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (!mKeyboard->isKeyDown(OIS::KC_A) && clientPlayer->checkState(Left))
    // {
    //     clientPlayer->setState(Left, false);
    //     pack.id = 'a';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (!mKeyboard->isKeyDown(OIS::KC_S) && clientPlayer->checkState(Back))
    // {
    //     clientPlayer->setState(Back, false);
    //     pack.id = 's';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (!mKeyboard->isKeyDown(OIS::KC_D) && clientPlayer->checkState(Right))
    // {   
    //     clientPlayer->setState(Right, false);
    //     pack.id = 'd';
    //     packList.push_back(pack);
    //     result = true;
    // }
    // if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && clientPlayer->checkState(Jump))
    // {
    //     clientPlayer->setState(Jump, false);
    //     pack.id = 'j';
    //     packList.push_back(pack);
    //     result = true;
    // }   
    // if (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) && clientPlayer->checkState(Boost))
    // {
    //     clientPlayer->setState(Boost, false);
    //     pack.id = 'b';
    //     packList.push_back(pack);
    //     result = true;
    // }

    return result;
}
//-------------------------------------------------------------------------------------
void Client::updateScene() // Receive packets and interpret them
{
	vector<MCP_Packet> packList;
	int i = 0;

    packList = gameNetwork->receivePacket(playerID);
    while (packList.size() > i && packList[i].packetID != 'n')
    {
        interpretServerPacket(packList[i]);
        i++;
    }
}
//-------------------------------------------------------------------------------------
void Client::updateCamera(Ogre::Real elapseTime)
{
	if (pCam->isInAimMode())
        pCam->update(elapseTime, clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    else
        pCam->update(elapseTime, clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());      
}
//-------------------------------------------------------------------------------------
void Client::interpretServerPacket(MCP_Packet pack)
{
    Ogre::Vector3 newPos;
    Ogre::Quaternion newQuat;

    printf("packet ID: %c\n\n", pack.packetID);

    /* BEGIN GAME */ 
    if(pack.packetID == (char)(((int)'0') + GAMESTATE))
    {
        gameStart = true;
    }
    /* UPDATE PLAYERS */
    if (pack.packetID == (char)(((int)'0') + PLAYER))
    {
        PLAYER_packet p;
        memcpy(&p, &pack, sizeof(PLAYER_packet));

        newPos = Ogre::Vector3(p.x, p.y, p.z);
        newQuat = p.orientation;
        int newPlayerID = p.playID - '0';
        printf("New Player ID: %d from %c\n\n", newPlayerID, p.playID);
        if (playerList[newPlayerID] == NULL)
        {
            char playerBuffer[25];
            sprintf(playerBuffer, "Player%d", newPlayerID);

            playerList[newPlayerID] = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), newPlayerID);
            numPlayers++;
        }

        playerList[newPlayerID]->getSceneNode()->_setDerivedPosition(newPos);
        playerList[newPlayerID]->getSceneNode()->_setDerivedOrientation(newQuat);
        playerList[newPlayerID]->getSceneNode()->needUpdate();
    }
    /* UPDATE DISK */
   if (pack.packetID == (char)(((int)'0') + DISK))
    {
        DISK_packet* p = (DISK_packet*)&pack;
        newPos = Ogre::Vector3(p->x, p->y, p->z);
        newQuat = p->orientation;

        if (gameDisk == NULL)
        {
            gameDisk = new Disk("Disk", cSceneMgr, NULL, -1.0f);
            gameDisk->particleNode->setVisible(true);
        }

        gameDisk->getSceneNode()->_setDerivedPosition(newPos);
        gameDisk->getSceneNode()->_setDerivedOrientation(newQuat);
        gameDisk->getSceneNode()->needUpdate();
    }
    // if(pack.packetID == 'D')
    // {
    //     clientPlayer->setHolding(true);
    // }

    // if(pack.packetID == 'P')
    // {
    //     Power->getSceneNode()->_setDerivedPosition(newPos);
    //     Power->getSceneNode()->needUpdate();
    // }
    // if(pack.packetID == 'S')
    // {
    //     Speed->getSceneNode()->_setDerivedPosition(newPos);
    //     Speed->getSceneNode()->needUpdate();
    // }
    // if(pack.packetID == 'J')
    // {
    //     JumpPower->getSceneNode()->_setDerivedPosition(newPos);
    //     JumpPower->getSceneNode()->needUpdate();
    // }
    // if(pack.packetID == 'R')
    // {
    //     Restore->getSceneNode()->_setDerivedPosition(newPos);
    //     Restore->getSceneNode()->needUpdate();
    // }
    // if (pack.packetID == 'H')
    // {
    //     gameRoom->hTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
    // }
    // if (pack.packetID == 'C')
    // {
    //     gameRoom->cTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
    // }
}
//-------------------------------------------------------------------------------------
bool Client::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    // Scroll wheel.
    if (evt.state.Z.rel)
        sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);

    Ogre::SceneNode* pSceneNode = clientPlayer->getSceneNode();
    Ogre::SceneNode* pSightNode = clientPlayer->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = clientPlayer->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;

    if (pCam->isInAimMode())
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate /2) * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
       	clientOrientationChange = true;
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        clientOrientationChange = true;
    }
    pSightNode->setPosition(pSightNode->getPosition() + sightHeight);
    pCamNode->setPosition(pCamNode->getPosition().x, pCamNode->getPosition().y, -pSightNode->getPosition().z);

    return true;
}
//-------------------------------------------------------------------------------------
Player* Client::getPlayer()
{
    return clientPlayer;
}