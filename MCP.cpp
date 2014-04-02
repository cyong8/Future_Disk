#include "MCP.h"

//-------------------------------------------------------------------------------------
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
    switch (buttonID)
    {
        case OIS::MB_Left:
            return CEGUI::LeftButton;
        
        case OIS::MB_Right:
            return CEGUI::RightButton;
            
        case OIS::MB_Middle:
            return CEGUI::MiddleButton;
            
        default:
            return CEGUI::LeftButton;
    }
}
//-------------------------------------------------------------------------------------
MCP::MCP(void)
{
}
//-------------------------------------------------------------------------------------
MCP::~MCP(void)
{
    gameMusic->musicDone();
    gameMusic = NULL;
    delete mRoot;
}
//-------------------------------------------------------------------------------------
void MCP::createScene(void)
{
    srand(time(0));

    gameMusic = new Music();    // Initialize Music
    gameMusic->playMusic("Start");

    hostPlayer = NULL;
    clientPlayer = NULL;    // An array for more than 2 players
    gameDisk = NULL;
    gameSimulator = NULL;
    clientOrientationChange = false;
    clientGameStart = false;
    clientServerIdentifier = 0;

    /******************** Movement Constants ******************/
    mMove = 3.0f;
    sprintFactor = 3.0f;
    timeSinceLastStateUpdate = 0.01f;

    /******************** GAME STATE FLAGS ********************/
    gamePause = false;
    gameStart = false;
    gameOver = false;
    vKeyDown = false;
    resetFlag = false;
    stopPlayer1 = false;
    player1Loss = false;
    stopPlayer2 = false;
    player2Loss = false;
    cViewModeToggle = false;    
    clientVKeyDown = false;
    mRotate = 0.1f;
    sceneRendered = 0;
    gameMode = 0;

    gui = new GUI(this); 
    gui->createMainMenu(); 
}
//-------------------------------------------------------------------------------------
void MCP::createSoloModeScene()
{
    gameSimulator = new Simulator(mSceneMgr, gameMusic);   // Initialize Simulator

    gameRoom = new Room(mSceneMgr, gameSimulator, clientServerIdentifier);

    /********************  OBJECT CREATION  ********************/
    pCam = new PlayerCamera("P1Cam", mSceneMgr, mCamera); 
    gameSimulator->setCamera(pCam); 
    (new Player("Player1", mSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, 15.0f), "Positive Side"))->addToSimulator(); // Create Player 1
    
    (new Target("Target1", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Target
    (new Target("Target2", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Target
    (new Target("Target3", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Target

    hostPlayer = (Player*)gameSimulator->getGameObject("Player1");
    //trajectory = mSceneMgr->createManualObject("Line");
    
    /********************    LIGHTS     ********************/
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = mSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameSimulator->getGameObject("Ceiling")->getSceneNode()->getPosition().y, 0.0f));
    
    createOverlays(pCam);
}
//-------------------------------------------------------------------------------------
void MCP::createMultiplayerModeScene_host()
{
    gameSimulator = new Simulator(mSceneMgr, gameMusic);   // Initialize Simulator

    gameRoom = new Room(mSceneMgr, gameSimulator, clientServerIdentifier);
    /********************  OBJECT CREATION  ********************/
    pCam = new PlayerCamera("P1Cam", mSceneMgr, mCamera); 
    gameSimulator->setCamera(pCam); 

    (new Player("Player1", mSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, 15.0f), "Positive Side"))->addToSimulator(); // Create Player 1
    (new Player("Player2", mSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, -15.0f), "Negative Side"))->addToSimulator(); // Create Player 2

    (new Target("Power", mSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Power-up
    (new Target("Speed", mSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Power-up
    (new Target("Jump", mSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Power-up
    (new Target("Restore", mSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()))->addToSimulator(); // Create initial Power-up

    Power = (Target*)gameSimulator->getGameObject("Power");
    Speed = (Target*)gameSimulator->getGameObject("Speed");
    JumpPower = (Target*)gameSimulator->getGameObject("Jump");
    Restore = (Target*)gameSimulator->getGameObject("Restore");

    hostPlayer = (Player*)gameSimulator->getGameObject("Player1");
    clientPlayer = (Player*)gameSimulator->getGameObject("Player2");

    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = mSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameSimulator->getGameObject("Ceiling")->getSceneNode()->getPosition().y, 0.0f));
    
    createOverlays(pCam);
}
//-------------------------------------------------------------------------------------
void MCP::createMultiplayerModeScene_client()
{
    gameRoom = new Room(mSceneMgr, NULL, clientServerIdentifier);
    pCam = new PlayerCamera("P2Cam", mSceneMgr, mCamera); 

    hostPlayer = new Player("Player1", mSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, 15.0f), "Positive Side");
    clientPlayer = new Player("Player2", mSceneMgr, NULL, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, -15.0f), "Negative Side");

    Power = new Target("Power", mSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    Speed = new Target("Speed", mSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    JumpPower = new Target("Jump", mSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up
    Restore = new Target("Restore", mSceneMgr, NULL, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds()); // Create initial Power-up

    pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(clientPlayer);

    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create Light for room
    pointLight = mSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getHeight()/2, 0.0f));

    createOverlays(pCam);
}
//-------------------------------------------------------------------------------------
bool MCP::soloMode(const CEGUI::EventArgs &e)
{
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);
    gui->removePanel(gameOverPanel);
    gui->removePanel(gameOverWinPanel);
    gui->removePanel(gameOverLossPanel); 
       
    time(&initTime);     

    gui->addPanel(scorePanel, OgreBites::TL_BOTTOMRIGHT);

    clientServerIdentifier = 0;
    gameMusic->playMusic("Play");
    gameOver = false;
    gameStart = true;
    gameMode = 0;
    score = 0;
    
    if (!resetFlag)
        createSoloModeScene();    
    
    resetFlag = true;
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::hostGame(const CEGUI::EventArgs &e)
{
    gameMode = 1;

    clientServerIdentifier = 0;

    gameNetwork = new Network(clientServerIdentifier, NULL); // Initialize Network

    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();

    gameMusic->playMusic("Play");

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);
    gui->removePanel(powerUpPanel);
    gui->removePanel(gameOverPanel);
    gui->removePanel(gameOverWinPanel);
    gui->removePanel(gameOverLossPanel);

    gameOver = false;
    score = 0;
    time(&initTime);

    createMultiplayerModeScene_host();
    sceneRendered = 1;
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::joinGame(const CEGUI::EventArgs &e)
{
    gameMode = 1;
    
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::WindowManager::WindowIterator wIterator = wmgr.getIterator();
    
    wIterator++;
  
    std::string ip_string = wIterator.getCurrentValue()->getText().c_str();
    char* ip = new char[ip_string.length()+1];
    std::memcpy(ip, wIterator.getCurrentValue()->getText().c_str(), ip_string.length()+1);
    //std::strcpy(ip, (const char*) ip_string);

    clientServerIdentifier = 1;
    gameNetwork = new Network(clientServerIdentifier, ip); // Initialize Network

    CEGUI::MouseCursor::getSingleton().hide();
    wmgr.destroyAllWindows();

    gameMusic->playMusic("Play");

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);
    gui->removePanel(powerUpPanel);
    gui->removePanel(gameOverPanel);
    gui->removePanel(gameOverWinPanel);
    gui->removePanel(gameOverLossPanel);
      
    gameOver = false;
    score = 0;
    time(&initTime);

    createMultiplayerModeScene_client();
    sceneRendered = 1;
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    static int count = 0;
    bool ret = BaseApplication::frameRenderingQueued(evt);
    
    if (hostPlayer != NULL && checkGameLoss(hostPlayer)) {
        player1Loss = true;
        gameOver = true;
    }
        
    if (clientPlayer != NULL && checkGameLoss(clientPlayer)) {
        player2Loss = true;
        gameOver = true;
    }

    if (mShutDown)
        return false;
    if (gameMode == 1) // attempt to establish the connection
    {
        if (!gameNetwork->checkConnection())
        {
            if (!gameNetwork->establishConnection())
                return true;
            else 
                gameStart = true;
        }
        else if (!gameStart)
            gameStart = true;
    }
    if(!gameStart && !gameOver) // Game not started
    {
        gui->removeLabel(pauseLabel);
        gui->removePanel(gameOverPanel);
        gui->removePanel(gameOverWinPanel);
        gui->removePanel(gameOverLossPanel); 
    }
    else if(gameOver)
    {
        if (gameMode == 1) {
            if (player1Loss) {
                if (clientServerIdentifier == 0) {
                    gui->addPanel(gameOverLossPanel, OgreBites::TL_CENTER);
                }
                else {
                    gui->addPanel(gameOverWinPanel, OgreBites::TL_CENTER);
                }
            }
            else if (player2Loss) {
                if (clientServerIdentifier == 0) {
                    gui->addPanel(gameOverWinPanel, OgreBites::TL_CENTER);
                }
                else {
                    gui->addPanel(gameOverLossPanel, OgreBites::TL_CENTER);
                }
            }
        }
        else {
            gui->addPanel(gameOverPanel, OgreBites::TL_CENTER);
            gameOverPanel->setParamValue(1, Ogre::StringConverter::toString(score));
        }
        if (gameStart && clientServerIdentifier == 0)
            gui->gameOverScreen();
        else if (gameStart && clientServerIdentifier == 1)
            gui->otherGameOverScreen();
        gameStart = false;
    }
    else // Game started
    {
        if(!gamePause)
        {
            if(gameMode == 0)    // Limit Solo mode to option of pausing
            {
                time_t currTime;
                time(&currTime);
                gameOver = updateTimer(currTime);
                if (gameOver)
                    gameMusic->playMusic("Pause");
                modifyScore(gameSimulator->tallyScore());
            }
            if (clientServerIdentifier == 0)     // Host render loop - Specific processing of inputs
            {
                renderLoop_Host(evt);
            }
            else if (clientServerIdentifier == 1)    // Client render loop - Specific processing of inputs
            {
                renderLoop_Client(evt);
            }     
        }
        else
        {
                    time_t pcurrTime;
                    time(&pcurrTime);
                    updatePauseTime(pcurrTime);
        }
    }
    return ret;
}
//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    static bool spacePressedLast = false;
    static Ogre::Real timeSinceLastJump = 0.0f;
    bool turboMode = false;
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    Player *p = (Player *)gameSimulator->getGameObject("Player1");     // Get the player object from the simulator

    float fx = 0.0f;                                                   // Force x-component
    float fz = 0.0f;                                                   // Force z-component
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);            // Initial velocity vector

    timeSinceLastJump += evt.timeSinceLastFrame;

    /********************     MOVEMENT   ********************/
    // Allow movement if the player is on the floor and the game is not paused
    if(!gamePause && gameSimulator->checkGameStart())
    {
        // If the mouse button was not pressed in the last frame, the mouse is pressed in the current frame, and the player is holding the disk then they are trying to throw
        if(!mMouseDown && currMouse && p->checkHolding() && vKeyDown) 
        {
            gameMusic->playMusic("Throw");
            gameSimulator->setThrowFlag();
            p->getPlayerDisk()->getSceneNode()->setVisible(true, true);
        }
        mMouseDown = currMouse; // Set that the mouse WAS pressed
        
        if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeyDown) // if 'v' is pressed and was not pressed last frame - go to aim mode
        {
            PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
            gameSimulator->toggleViewChange("Player1");
            pc->toggleThirdPersonView();
            vKeyDown = true;
            //showTrajectory(pc);
        }
        if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown) // if 'v' is not pressed and was pressed last frame - exit aim mode
        {
            PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
            gameSimulator->toggleViewChange("Player1");
            pc->toggleThirdPersonView();
            vKeyDown = false;
            //mSceneMgr->getRootSceneNode()->detachObject(trajectory);
            //trajectory->clear();
        }
        if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)) // Move into Boost mode
        {
            turboMode = true;
        }
        // If the 'V' key is down you shouldn't be able to move
        if (!vKeyDown)  
        {
             // Move the player
            if (mKeyboard->isKeyDown(OIS::KC_W)) // Forward
            {
                fz -= mMove;
                velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
                keyWasPressed = true;
            }
            if (mKeyboard->isKeyDown(OIS::KC_S)) // Backward
            {
                fz += mMove;
                velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
                keyWasPressed = true;
            }

            if (mKeyboard->isKeyDown(OIS::KC_A)) // Left - yaw or strafe
            {
                fx -= mMove; // Strafe left
                velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
                keyWasPressed = true;
            }
            if (mKeyboard->isKeyDown(OIS::KC_D)) // Right - yaw or strafe
            {
                fx += mMove; // Strafe right
                velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
                keyWasPressed = true;
            }
            if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !p->groundConstantSet && !spacePressedLast) 
            {
                if(p->performJump())
                {
                    if (p->jumpPowerActive)
                        gameMusic->playMusic("SuperJump");
                    else
                        gameMusic->playMusic("Jump");
                    spacePressedLast = true;
                    gameSimulator->soundedJump = true;
                }
            }
            if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && spacePressedLast)
                spacePressedLast = false;
            if(keyWasPressed)
            {   // Rotate the velocity vector by the orientation of the player
                Ogre::Vector3 trueVelocity = Ogre::Vector3(velocityVector.getX(), velocityVector.getY(), velocityVector.getZ());
                trueVelocity = p->getSceneNode()->getOrientation() * trueVelocity; 
                btVector3 btTrueVelocity = btVector3(trueVelocity.x, trueVelocity.y, trueVelocity.z);

                if (p->getGameObjectName() == "Player1") {
                    if (!stopPlayer1) {
                        if (turboMode)
                            p->getBody()->setLinearVelocity((btTrueVelocity * sprintFactor) + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
                        else
                            p->getBody()->setLinearVelocity(btTrueVelocity + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
                    }
                    else {
                        p->getBody()->setLinearVelocity(btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.5f));
                    }
                }
                else if (p->getGameObjectName() == "Player2") {
                    if (!stopPlayer2) {
                        if (turboMode)
                            p->getBody()->setLinearVelocity((btTrueVelocity * sprintFactor) + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
                        else
                            p->getBody()->setLinearVelocity(btTrueVelocity + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
                    }
                    else {
                        p->getBody()->setLinearVelocity(btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), -0.5f));
                    }
                }   
            }
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------
void MCP::renderLoop_Host(const Ogre::FrameEvent& evt)
{
    if(!processUnbufferedInput(evt)) 
        exit(2);

    gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f); 
    gameSimulator->parseCollisions(); // check collisions

    updateRemovedTiles();

    if (sceneRendered)
    {
        if (mShutDown)
           exit(2);                    
        if (timeSinceLastStateUpdate < 0.0f)
        {
            constructAndSendGameState();
        }
        if (gameNetwork->checkSockets() && clientGameStart)
        {
            int i = 0;
            vector<MCP_Packet> packList;
            packList = gameNetwork->receivePacket();
            //printf("pack.id (before while) = %c\n\n", packList[0].id);
           
            while (packList.size() > i && packList[i].id != 'n')
            {
                printf("pack.id (inside while) = %c\n\n", packList[i].id);
                interpretClientPacket(packList[i]);
                i++;
            }
        }
        printf("\t\t Client States: W - %d\n", hostPlayer->checkState(Forward));
        printf("\t\t Client States: A - %d\n", hostPlayer->checkState(Left));
        printf("\t\t Client States: S - %d\n", hostPlayer->checkState(Back));
        printf("\t\t Client States: D - %d\n", hostPlayer->checkState(Right));
        printf("\t\t Client States: Shift - %d\n", hostPlayer->checkState(Boost));
        if (gameSimulator->checkGameStart() && !clientVKeyDown)
        {
            Ogre::Vector3 velocityVector;
            velocityVector = hostPlayer->fillClientVelocityVector(mMove, sprintFactor);
            velocityVector = clientPlayer->getSceneNode()->getOrientation() * velocityVector; 
            btVector3 btTrueVelocity = btVector3(velocityVector.x, velocityVector.y, velocityVector.z);

            clientPlayer->getBody()->setLinearVelocity(btTrueVelocity + (btVector3(0.0f, clientPlayer->getBody()->getLinearVelocity().getY(), 0.0f)));
        }
        // Update client velocity 
        //printf("Time Since Last Update: %f\n\n", timeSinceLastStateUpdate);
        if (timeSinceLastStateUpdate < 0.0f)
            timeSinceLastStateUpdate = 0.01f;
        timeSinceLastStateUpdate -= evt.timeSinceLastFrame;
    }
    // if (gameMode == 1)
    // {
    //     if (hostPlayer != NULL)
    //         restrictPlayerMovement(hostPlayer);
    //     if (clientPlayer != NULL)
    //         restrictPlayerMovement(clientPlayer);
    // }
    if (gameSimulator->setDisk && gameSimulator->gameDisk == NULL)
    {
        (new Disk("Disk", mSceneMgr, gameSimulator, 0.0f/*Ogre::Math::RangeRandom(0,2)*/))->addToSimulator();
        gameDisk = (Disk*)gameSimulator->getGameObject("Disk");
        //gameDisk->particleNode->setVisible(true);
    }
}
//-------------------------------------------------------------------------------------
void MCP::renderLoop_Client(const Ogre::FrameEvent& evt)
{
    if (mShutDown)
        exit(2);
    if (sceneRendered)
    {
        if (clientGameStart /*&& timeSinceLastStateUpdate < 0.00f*/)
            processAndSendClientInput(evt);
        if (gameNetwork->checkSockets())
            updateClient(evt);
        updateClientCamera(evt.timeSinceLastFrame);
        timeSinceLastStateUpdate -= evt.timeSinceLastFrame;
        //printf("Time Since Last Update: %f\n\n", timeSinceLastStateUpdate);
        if (timeSinceLastStateUpdate < 0.0f)
            timeSinceLastStateUpdate = 0.01f;
    }
}
//-------------------------------------------------------------------------------------
bool MCP::constructAndSendGameState()   
{
    vector<MCP_Packet> packList;
    MCP_Packet pack;
    memset(&pack, 0, sizeof(MCP_Packet));

    // Update hostPlayer
    pack.id = 'h';
    pack.x_coordinate = hostPlayer->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = hostPlayer->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = hostPlayer->getSceneNode()->_getDerivedPosition().z;
    pack.orientationQ = hostPlayer->getSceneNode()->_getDerivedOrientation();
    packList.push_back(pack);

    // Update clientPlayer
    pack.id = 'c';
    pack.x_coordinate = clientPlayer->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = clientPlayer->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = clientPlayer->getSceneNode()->_getDerivedPosition().z;
    packList.push_back(pack);

    pack.id = 'P';
    pack.x_coordinate = Power->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = Power->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = Power->getSceneNode()->_getDerivedPosition().z;
    packList.push_back(pack);

    pack.id = 'S';
    pack.x_coordinate = Speed->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = Speed->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = Speed->getSceneNode()->_getDerivedPosition().z;
    packList.push_back(pack);

    pack.id = 'J';
    pack.x_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = JumpPower->getSceneNode()->_getDerivedPosition().z;
    packList.push_back(pack);

    pack.id = 'R';
    pack.x_coordinate = Restore->getSceneNode()->_getDerivedPosition().x;
    pack.y_coordinate = Restore->getSceneNode()->_getDerivedPosition().y;
    pack.z_coordinate = Restore->getSceneNode()->_getDerivedPosition().z;
    packList.push_back(pack);

    int rIndex;
    while (removedCTileList.size() != 0)
    {
        pack.id = 'C';

        rIndex = removedCTileList[removedCTileList.size() - 1];
        removedCTileList.pop_back();

        pack.tileIndex = rIndex;
        packList.push_back(pack);
    }
    while (removedHTileList.size() != 0)
    {
        pack.id = 'H';   

        rIndex = removedHTileList[removedHTileList.size() - 1];
        removedHTileList.pop_back();

        pack.tileIndex = rIndex;
        packList.push_back(pack);
    }

    if (gameDisk != NULL)
    {
        if (clientPlayer->checkHolding())
        {
            pack.id = 'D';
            packList.push_back(pack);
        }
        pack.id = 'd';
        pack.x_coordinate = gameDisk->getSceneNode()->_getDerivedPosition().x;
        pack.y_coordinate = gameDisk->getSceneNode()->_getDerivedPosition().y;
        pack.z_coordinate = gameDisk->getSceneNode()->_getDerivedPosition().z;
        pack.orientationQ = gameDisk->getSceneNode()->_getDerivedOrientation();
        packList.push_back(pack);
    }
    if (gameSimulator->checkGameStart() && !clientGameStart)
    {
        pack.id = 's';   
        packList.push_back(pack);
        clientGameStart = true;
    }

    pack.id = 'n';
    packList.push_back(pack);

    gameNetwork->sendPacket(packList);
}
//-------------------------------------------------------------------------------------
bool MCP::updateClient(const Ogre::FrameEvent& evt)
{
    vector<MCP_Packet> packList;
    int i = 0;
    // INTERPRETS PACKET
    packList = gameNetwork->receivePacket();
    while ( packList.size() > i && packList[i].id != 'n')
    {
        interpretServerPacket(packList[i]);
        i++;
    }
    //INDIVIDUAL INPUT - SEND PACKETS
    return true;
}
//-------------------------------------------------------------------------------------
void MCP::updateClientCamera(Ogre::Real elapseTime)
{
    if (cViewModeToggle) // View was toggled; now check what view it needs to be changed to
    {
        cViewModeToggle = !cViewModeToggle;

        if(pCam->isInAimMode()) // Go into Aim view
        {
            gameDisk->getSceneNode()->setVisible(false);
            pCam->initializePosition(clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
        }
        else // Return from Aim view
        {
            gameDisk->getSceneNode()->setVisible(true);
            pCam->initializePosition(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
        }
    }
    else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
    {
        if (pCam->isInAimMode())
            pCam->update(elapseTime, clientPlayer->getSceneNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
        else
            pCam->update(elapseTime, clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());          
    }
}
//-------------------------------------------------------------------------------------
bool MCP::processAndSendClientInput(const Ogre::FrameEvent& evt)
{
    vector<MCP_Packet> packList;
    MCP_Packet pack;
    bool result = false;
    memset(&pack, 0, sizeof(MCP_Packet));

    if (clientOrientationChange)
    {
        clientOrientationChange = false;
        pack.id = 'o';
        pack.orientationQ = clientPlayer->getSceneNode()->_getDerivedOrientation();
        packList.push_back(pack);
        pack.id = 'S';
        pack.x_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().x;
        pack.y_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().y;
        pack.z_coordinate = clientPlayer->getPlayerSightNode()->_getDerivedPosition().z;
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        mShutDown = true;
        return false;
    }
    if (mKeyboard->isKeyDown(OIS::KC_W) && !clientPlayer->checkState(Forward))                              // Forward - implemented
    {
        clientPlayer->toggleState(Forward, true);
        pack.id = 'w';
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_A) && !clientPlayer->checkState(Left))                                 // Left - implemented
    {
        clientPlayer->toggleState(Left, true);
        pack.id = 'a';
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_S) && !clientPlayer->checkState(Back))                                 // Backwards - implemented
    {
        clientPlayer->toggleState(Back, true);
        pack.id = 's';
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_D) && !clientPlayer->checkState(Right))                                 // Right - implemented
    {
        clientPlayer->toggleState(Right, true);
        pack.id = 'd';
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !clientPlayer->checkState(Jump))   // Jump - implemented
    {
        clientPlayer->toggleState(Jump, true);
        pack.id = 'j';
        packList.push_back(pack);
        result = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_V) && !clientVKeyDown)            // Aim View Toggle - Send to Server so they can let you throw; update camera position on client end
    {
        cViewModeToggle = true;  // toggle - always true
        pCam->toggleThirdPersonView();
        clientVKeyDown = true;
        pack.id = 'v';
        packList.push_back(pack);
        result = true;
    }
    if (!mKeyboard->isKeyDown(OIS::KC_V) && clientVKeyDown)        
    {
        cViewModeToggle = true;     // toggle - always true
        pCam->toggleThirdPersonView();
        clientVKeyDown = false;
        pack.id = 'v';
        packList.push_back(pack);
        result = true;
    }    
    if (mKeyboard->isKeyDown(OIS::KC_LSHIFT) && !clientPlayer->checkState(Boost))                          // Speed Boost
    {
        clientPlayer->toggleState(Boost, true); 
        pack.id = 'b';
        packList.push_back(pack);
        result = true;
    }
    if (mMouse->getMouseState().buttonDown(OIS::MB_Left) && clientVKeyDown && clientPlayer->checkHolding())
    {
        pack.id = 't';
        packList.push_back(pack);
        result = true;
        clientPlayer->isHolding = false;
    }
    if (resetClientState(evt, packList) || result)
    {
        pack.id = 'n';
        packList.push_back(pack);
        gameNetwork->sendPacket(packList);
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::resetClientState(const Ogre::FrameEvent& evt, vector<MCP_Packet> &packList)
{
    MCP_Packet pack;
    bool result = false;
    memset(&pack, 0, sizeof(MCP_Packet));

    if (!mKeyboard->isKeyDown(OIS::KC_W) && clientPlayer->checkState(Forward))
    {
        clientPlayer->toggleState(Forward, false);
        pack.id = 'w';
        packList.push_back(pack);
        result = true;
    }
    if (!mKeyboard->isKeyDown(OIS::KC_A) && clientPlayer->checkState(Left))
    {
        clientPlayer->toggleState(Left, false);
        pack.id = 'a';
        packList.push_back(pack);
        result = true;
    }
    if (!mKeyboard->isKeyDown(OIS::KC_S) && clientPlayer->checkState(Back))
    {
        clientPlayer->toggleState(Back, false);
        pack.id = 's';
        packList.push_back(pack);
        result = true;
    }
    if (!mKeyboard->isKeyDown(OIS::KC_D) && clientPlayer->checkState(Right))
    {   
        clientPlayer->toggleState(Right, false);
        pack.id = 'd';
        packList.push_back(pack);
        result = true;
    }
    if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && clientPlayer->checkState(Jump))
    {
        clientPlayer->toggleState(Jump, false);
        pack.id = 'j';
        packList.push_back(pack);
        result = true;
    }   
    if (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) && clientPlayer->checkState(Boost))
    {
        clientPlayer->toggleState(Boost, false);
        pack.id = 'b';
        packList.push_back(pack);
        result = true;
    }

    return result;
}
//-------------------------------------------------------------------------------------
bool MCP::interpretClientPacket(MCP_Packet pack)
{
    // Update the player rigid body and scenenode - Note: The states[] of the host tracks the client state; not the host state
    char typeInput = pack.id;
    
//    printf("\t\t*****Client sending sequence %c\n\n", pack.id);
    
    if (typeInput == 'w')                                       // Forward
    {
        if (hostPlayer->checkState(Forward))
            hostPlayer->toggleState(Forward, false);
        else
            hostPlayer->toggleState(Forward, true);
    }
    if (typeInput == 'a')                                       // Left
    {
        if (hostPlayer->checkState(Left))
            hostPlayer->toggleState(Left, false);
        else
            hostPlayer->toggleState(Left, true);
    }
    if (typeInput == 's')                                       // Backwards
    {
        if (hostPlayer->checkState(Back))
            hostPlayer->toggleState(Back, false);
        else
            hostPlayer->toggleState(Back, true);
    }
    if (typeInput == 'd')                                       // Right
    {
        if (hostPlayer->checkState(Right))
            hostPlayer->toggleState(Right, false);
        else
            hostPlayer->toggleState(Right, true);
    }
    if (typeInput == 'j' && !clientPlayer->groundConstantSet)   // Jump
    {
        clientPlayer->performJump();
    }
    if (typeInput == 'v')                                       // View Mode Toggle
    {
        clientVKeyDown = !clientVKeyDown;
    }
    if (typeInput == 'b')                                       // speed boost
    {
        if (hostPlayer->checkState(Boost))
            hostPlayer->toggleState(Boost, false);
        else
            hostPlayer->toggleState(Boost, true);
    }
    if (typeInput == 't' && clientPlayer->checkHolding())       // Player tried to throw
    {
        gameSimulator->setThrowFlag();
    }
    if (typeInput == 'o')
    {
        clientPlayer->getSceneNode()->_setDerivedOrientation(pack.orientationQ);

        btQuaternion rotationQ;
        btTransform transform = clientPlayer->getBody()->getCenterOfMassTransform();

        rotationQ = btQuaternion(clientPlayer->getSceneNode()->getOrientation().getYaw().valueRadians(), 0, 0);
        transform.setRotation(rotationQ);

        clientPlayer->getBody()->setCenterOfMassTransform(transform);
    }
    if (typeInput == 'S')
    {
        clientPlayer->getPlayerSightNode()->setPosition(Ogre::Vector3(pack.x_coordinate, pack.y_coordinate, pack.z_coordinate));
    }

    return false;
}
//-------------------------------------------------------------------------------------
bool MCP::interpretServerPacket(MCP_Packet pack)
{
    Ogre::Vector3 newPos;
    Ogre::Quaternion newQuat;
    newPos = Ogre::Vector3(pack.x_coordinate, pack.y_coordinate, pack.z_coordinate);
    newQuat = pack.orientationQ;

    if(pack.id == 's')
    {
        clientGameStart = true;
    }

    if (pack.id == 'h')
    {
        hostPlayer->getSceneNode()->_setDerivedPosition(newPos);
        hostPlayer->getSceneNode()->_setDerivedOrientation(newQuat);
        hostPlayer->getSceneNode()->needUpdate();
    }
    if (pack.id == 'c')   
    {
        clientPlayer->getSceneNode()->_setDerivedPosition(newPos);
        clientPlayer->getSceneNode()->needUpdate();
    }
    if (pack.id == 'd')
    {
        if (gameDisk == NULL)
        {
            gameDisk = new Disk("Disk", mSceneMgr, gameSimulator, -1.0f);
            gameDisk->particleNode->setVisible(true);
        }

        gameDisk->getSceneNode()->_setDerivedPosition(newPos);
        gameDisk->getSceneNode()->_setDerivedOrientation(newQuat);
        gameDisk->getSceneNode()->needUpdate();
    }
    if(pack.id == 'D')
    {
        clientPlayer->isHolding = true;
    }

    if(pack.id == 'P')
    {
        Power->getSceneNode()->_setDerivedPosition(newPos);
        Power->getSceneNode()->needUpdate();
    }
    if(pack.id == 'S')
    {
        Speed->getSceneNode()->_setDerivedPosition(newPos);
        Speed->getSceneNode()->needUpdate();
    }
    if(pack.id == 'J')
    {
        JumpPower->getSceneNode()->_setDerivedPosition(newPos);
        JumpPower->getSceneNode()->needUpdate();
    }
    if(pack.id == 'R')
    {
        Restore->getSceneNode()->_setDerivedPosition(newPos);
        Restore->getSceneNode()->needUpdate();
    }
    if (pack.id == 'H')
    {
        gameRoom->hTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
    }
    if (pack.id == 'C')
    {
        gameRoom->cTileList[pack.tileIndex]->getSceneNode()->setVisible(false);
    }

    return true;
}
//-------------------------------------------------------------------------------------
void MCP::updateRemovedTiles()
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
bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    // Scroll wheel.
    if (evt.state.Z.rel)
        sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
    
    // if (gameSimulator != NULL)
    // {
    //     if (!gameSimulator->checkGameStart())
    //         return false;
    // }

    if (!gameStart || gamePause) // restrict movements before the game has started or during pause
        return false;

    Player* p;    
    if(clientServerIdentifier == 0)
        p = hostPlayer;
    if(clientServerIdentifier == 1)
        p = clientPlayer;

    Ogre::SceneNode* pSceneNode = p->getSceneNode();
    Ogre::SceneNode* pSightNode = p->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = p->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;

    btRigidBody* pBody;
    btTransform transform;
    btQuaternion rotationQ;

    if (clientServerIdentifier == 0)
    {
        pBody = p->getBody();
        transform = pBody->getCenterOfMassTransform();
    }
    if (clientServerIdentifier == 1 && gameDisk == NULL)
        return true;
    if (vKeyDown)
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate/2) * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        if (clientServerIdentifier == 0)
        {
            rotationQ = btQuaternion(pSceneNode->getOrientation().getYaw().valueRadians(), 0, 0);
            transform.setRotation(rotationQ);
            pBody->setCenterOfMassTransform(transform);
        }
        else
            clientOrientationChange = true;
        /*if (p->checkHolding()) {
            mSceneMgr->getRootSceneNode()->detachObject(trajectory);
            trajectory->clear();
            showTrajectory(pCam);
        }*/
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        if (clientServerIdentifier == 0)
        {
            rotationQ = btQuaternion(pSceneNode->getOrientation().getYaw().valueRadians(), 0, 0);
            transform.setRotation(rotationQ);
            pBody->setCenterOfMassTransform(transform);
        }
        else
            clientOrientationChange = true;
    }
    // p->getPlayerCameraNode()->setPosition(p->getPlayerCameraNode()->getPosition() + Ogre::Vector3(0.0f, 0.0f, 12.5f));
    pSightNode->setPosition(pSightNode->getPosition() + sightHeight);
    pCamNode->setPosition(pCamNode->getPosition().x, pCamNode->getPosition().y, -pSightNode->getPosition().z);

    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::keyPressed(const OIS::KeyEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectKeyDown(evt.key);
    sys.injectChar(evt.text);
    
    switch (evt.key)
    {
        case OIS::KC_ESCAPE:
            mShutDown = true;
            break;
        case OIS::KC_SYSRQ:
            mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
            break;
        case OIS::KC_M:
            gameMusic->toggleMute();
            break;
        case OIS::KC_P:
            if (gameMode == 0)
                togglePause();
            break;
    }
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::keyReleased(const OIS::KeyEvent &evt)
{
    CEGUI::System::getSingleton().injectKeyUp(evt.key);
    return true;
}
//-------------------------------------------------------------------------------------
void MCP::togglePause()
{
    gui->pauseMenu(gamePause);
    
    if (gamePause == true)  //leaving pause
    {        
        gameMusic->playMusic("Play");
        gamePause = false;
        gui->removeLabel(pauseLabel);
        gui->removePanel(objectivePanel);
        gui->removePanel(instructPanel);
    }
    else //entering Pause
    {        
        gameMusic->playMusic("Start");
        pauseLabel->setCaption("GAME PAUSED!");
        gui->addLabel(pauseLabel, OgreBites::TL_CENTER);
        gui->addPanel(objectivePanel, OgreBites::TL_TOP);
        gui->addPanel(instructPanel, OgreBites::TL_RIGHT);
        gamePause = true;
        time(&pauseTime);
    }
} 
//-------------------------------------------------------------------------------------
bool MCP::checkGameLoss(Player* p) {
    Ogre::Vector3 pos = p->getSceneNode()->getPosition();
    
    if (pos.y < -40.0f)
        return true;
    return false;
}
//-----------------------------------------------------------------------------------
bool MCP::activateMainMenuSolo(const CEGUI::EventArgs &e) 
{   
    gui->removePanel(scorePanel);
    //gui->removePanel(gameOverPanel);
    gameOver = false;
    
    gui->addPanel(instructPanel, OgreBites::TL_RIGHT);
    gui->addPanel(objectivePanel, OgreBites::TL_TOP);
    gui->createMainMenu();
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::quit(const CEGUI::EventArgs &e)
{
    mShutDown = true;
    return true;
}
//-------------------------------------------------------------------------------------
void MCP::createOverlays(PlayerCamera* playCam)
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

    playCam->setCHOverlays(crossHairVertOverlay, crossHairHorizOverlay); // WDTD
}
//-------------------------------------------------------------------------------------
void MCP::restrictPlayerMovement(Player* p)
{
    Ogre::Vector3 pos = p->getSceneNode()->getPosition();
    Ogre::Vector3 dim = p->getPlayerDimensions();

    // Left/Right wall check
    //if ((pos.x + dim.x) >= gameRoom->getWidth()/2.0f || (pos.x + dim.x) <= -gameRoom->getWidth()/2.0f)
        //p->getBody()->setLinearVelocity(btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), p->getBody()->getLinearVelocity().getZ()));
    // Gap and (Relative) Back wall check
    //if ((pos.z + dim.z) >= gameRoom->getHeight()/2.0f || (pos.z + dim.z) <= -gameRoom->getHeight()/2.0f)
        //p->getBody()->setLinearVelocity(btVector3(p->getBody()->getLinearVelocity().getX(), p->getBody()->getLinearVelocity().getY(), 0.0f));
        
    if (p->getGameObjectName() == "Player1" && (pos.z + dim.z) <= 7.0f)
        stopPlayer1 = true;
    else
        stopPlayer1 = false;
    if (p->getGameObjectName() == "Player2" && (pos.z + dim.z) >= -7.0f)
        stopPlayer2 = true;
    else
        stopPlayer2 = false;
}
//-------------------------------------------------------------------------------------
/*void MCP::showTrajectory(PlayerCamera* playCam)
{
    Ogre::Vector3 init_pos = playCam->getPlayer()->getSceneNode()->getPosition();
    Ogre::Vector3 init_sight_pos = playCam->getPlayer()->getPlayerSightNode()->getPosition();
    
    trajectory->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);
    
    trajectory->position(init_pos);
    trajectory->position(init_sight_pos);
    //trajectory->position(init_dir.x + 40, init_dir.y + 200, init_dir.z + 140);
    trajectory->end();   
    
    mSceneMgr->getRootSceneNode()->attachObject(trajectory);
}*/
//-------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif
 
#ifdef __cplusplus
extern "C" {
#endif
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        MCP app;
        for (int i = 0; i < argc; i++)
        {
            app.termArgs.push_back(argv[i]);
        }

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
 
        return 0;
    }
 
#ifdef __cplusplus
}
#endif
