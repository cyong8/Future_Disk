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

    /* GAME ROOM */
    gameRoom = new Room(cSceneMgr, NULL, 1);
    printf("\n\nPlayer ID: %d\n\n", playerID);

    /* CLIENT PLAYER */
    sprintf(playerBuffer, "Player%d", playerID);
    clientPlayer = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), playerID);
    playerList[playerID-1] = clientPlayer;
    numPlayers++;

    /* CLIENT'S PLAYER CAMERA */
	sprintf(cameraBuffer, "Player%dCam", playerID);
    pCam = new PlayerCamera(cameraBuffer, cSceneMgr, cSceneMgr->getCamera("PlayerCam"));
    pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(clientPlayer);
    
    /* TARGETS */
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
    if (gameNetwork->checkSockets(0))
    {
        updateScene();
    }

    if (!processUnbufferedInput(evt, mKeyboard, mMouse))
    {
        // send a packet telling the server you're leaving
        exit(2);
    }

    updateCamera(evt.timeSinceLastFrame);
}
//-------------------------------------------------------------------------------------
bool Client::processUnbufferedInput(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    char* buff;
    static bool vKeydown = false;

    if (clientOrientationChange)
    {
        clientOrientationChange = false;
       
        C_PLAYER_packet pack;
        pack.packetID =(char)(((int)'0') + C_PLAYER);
        pack.playID = (char)(((int)'0') + playerID);
        pack.orientation = clientPlayer->getSceneNode()->_getDerivedOrientation();
       
        buff = (char*)malloc(sizeof(C_PLAYER_packet));
        memcpy(buff, &pack, sizeof(C_PLAYER_packet));

        gameNetwork->sendPacket(buff, playerID);
        // pack.id = 'S';
        // pack.x_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().x;
        // pack.y_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().y;
        // pack.z_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().z;
        // packList.push_back(pack);
    }
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
void Client::updateScene() // Receive packets and interpret them
{
	char* packList = gameNetwork->receivePacket(playerID);
    
    interpretServerPacket(packList);
}
//-------------------------------------------------------------------------------------
void Client::updateCamera(Ogre::Real elapseTime)
{
	if (pCam->isInAimMode())
    {
        pCam->update(elapseTime, clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
    else
    {
        pCam->update(elapseTime, clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    }
}
//-------------------------------------------------------------------------------------
void Client::interpretServerPacket(char* packList)
{
    int indexIntoBuff = 0;
    Ogre::Vector3 newPos;
    Ogre::Quaternion newQuat;
    
    while (indexIntoBuff < MAX_SIZE_OF_BUFFER && packList[indexIntoBuff] != 0x00)
    {
        // printf("INTERPRETTING A PACKET!\n\n");
        char packType = packList[indexIntoBuff];
        /* UPDATE DISK */
        if (packType == (char)(((int)'0') + DISK))
        {
            printf("updating disk\n");
            DISK_packet d;
            memcpy(&d, packList + indexIntoBuff, sizeof(DISK_packet));

            newPos = Ogre::Vector3(d.x, d.y, d.z);
            newQuat = d.orientation;

            if (gameDisk == NULL)
            {
                gameDisk = new Disk("Disk", cSceneMgr, NULL, -1.0f);
                gameDisk->particleNode->setVisible(true);
            }

            gameDisk->getSceneNode()->_setDerivedPosition(newPos);
            gameDisk->getSceneNode()->_setDerivedOrientation(newQuat);
            gameDisk->getSceneNode()->needUpdate();

            indexIntoBuff += sizeof(DISK_packet);
        }
        /* UPDATE PLAYERS */
        // else if (packType == (char)(((int)'0') + S_PLAYER))
        else if (packType == (char)(((int)'0') + S_PLAYER))
        {
            S_PLAYER_packet p;

            memcpy(&p, packList, sizeof(S_PLAYER_packet));

            newPos = Ogre::Vector3(p.x, p.y, p.z);
            newQuat = p.orientation;

            int newPlayerID = p.playID - '0';
            if (playerList[newPlayerID] == NULL)
            {
                printf("*****ADDING NEW PLAYER\n\n");
                char playerBuffer[25];
                sprintf(playerBuffer, "Player%d", newPlayerID);

                playerList[newPlayerID] = new Player(playerBuffer, cSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), newPlayerID);
                numPlayers++;
            }
            printf("UPDATING PLAYER %d POSITION to Vector(%f, %f, %f)\n\n", newPlayerID, newPos.x, newPos.y, newPos.z);
            playerList[newPlayerID]->getSceneNode()->_setDerivedPosition(newPos);
            // playerList[newPlayerID]->getSceneNode()->_setDerivedOrientation(newQuat);
            // playerList[newPlayerID]->getSceneNode()->needUpdate();

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
}
//-------------------------------------------------------------------------------------
bool Client::mouseMoved(Ogre::Real relX, Ogre::Real relY)
{
    Ogre::SceneNode* pSceneNode = clientPlayer->getSceneNode();
    Ogre::SceneNode* pSightNode = clientPlayer->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = clientPlayer->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;


    printf("state relative X = %f, state relative y = %f\n\n", relX, relY);

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

    return true;
}
//-------------------------------------------------------------------------------------
Player* Client::getPlayer()
{
    return clientPlayer;
}