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

    hostPlayer = NULL;
    clientPlayer = NULL;

    gameMusic = new Music();    // Initialize Music
    gameMusic->playMusic("Start");

    gameSimulator = new Simulator(mSceneMgr, gameMusic);   // Initialize Simulator
    
    createGUI();
    
    /******************** GAME STATE FLAGS ********************/
    gamePause = false;
    gameStart = false;
    gameOver = false;
    vKeyDown = false;    
    mRotate = 0.1f;

    /* Create room and store player bounds */
    gameRoom = new Room(mSceneMgr, gameSimulator);

    createTargetModeScene();
    //createMultiplayerModeScene();
}
//-------------------------------------------------------------------------------------
void MCP::createTargetModeScene()
{
    /********************  OBJECT CREATION  ********************/
    PlayerCamera* pCam = new PlayerCamera("P1Cam", mSceneMgr, mCamera); 
    gameSimulator->setCamera(pCam); 
    (new Disk("Disk", mSceneMgr, gameSimulator, Ogre::Math::RangeRandom(0,1)))->addToSimulator(); 
    (new Player("Player1", mSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(1.0f, 1.0f, 15.0f), "Positive Side"))->addToSimulator(); // Create Player 1
    (new Target("Target1", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target2", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target3", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    gameDisk = (Disk*)gameSimulator->getGameObject("Disk");
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
void MCP::createMultiplayerModeScene()
{
    
}
//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static Ogre::Real mMove = 3.0f;                                    // The movement constant
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    static bool spacePressedLast = false;
    static Ogre::Real timeSinceLastJump = 0.0f;
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    Player *p = (Player *)gameSimulator->getGameObject("Player1");    // Get the player object from the simulator

    float fx = 0.0f;                                                   // Force x-component
    float fz = 0.0f;                                                   // Force z- component
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);            // Initial velocity vector
    
    float sprintFactor = 1.0f;                                         // How fast the character moves when Left Shift is held down
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
            sprintFactor = 3.0f;
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
                    gameMusic->playMusic("Jump");
                    spacePressedLast = true;
                    gameSimulator->soundedJump = true;
                }
            }
            if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && spacePressedLast)
                spacePressedLast = false;
            if(keyWasPressed == true)
            {   // Rotate the velocity vector by the orientation of the player
                Ogre::Vector3 trueVelocity = Ogre::Vector3(velocityVector.getX(), velocityVector.getY(), velocityVector.getZ());
                trueVelocity = p->getSceneNode()->getOrientation() * trueVelocity; 
                btVector3 btTrueVelocity = btVector3(trueVelocity.x, trueVelocity.y, trueVelocity.z);

                p->getBody()->setLinearVelocity((btTrueVelocity * sprintFactor) + (btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f)));
            }
        }
    }
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
  	sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
  	// Scroll wheel.
  	if (evt.state.Z.rel){
    	sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
  	}
  	
    if (!gameStart || gamePause) // restrict movements before the game has started or during pause
        return false;
    Player* p = (Player*)gameSimulator->getGameObject("Player1");
    PlayerCamera* pCam = gameSimulator->getPlayerCamera("P1Cam");
    Ogre::SceneNode* pSceneNode = p->getSceneNode();
    Ogre::SceneNode* pSightNode = p->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = p->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;
    btRigidBody* pBody = p->getBody();
    btTransform transform = pBody->getCenterOfMassTransform();
    btQuaternion rotationQ;

    /* rotation working, but camera not following */
    if (vKeyDown)
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate/2) * evt.state.X.rel), Ogre::Node::TS_WORLD);
        rotationQ = btQuaternion(pSceneNode->getOrientation().getYaw().valueRadians(), 0, 0);
        transform.setRotation(rotationQ);
        pBody->setCenterOfMassTransform(transform);

        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        
        /*if (p->checkHolding()) {
            mSceneMgr->getRootSceneNode()->detachObject(trajectory);
            trajectory->clear();
            showTrajectory(pCam);
        }*/
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * evt.state.X.rel), Ogre::Node::TS_WORLD);
        rotationQ = btQuaternion(pSceneNode->getOrientation().getYaw().valueRadians(), 0, 0);
        transform.setRotation(rotationQ);
        pBody->setCenterOfMassTransform(transform);

        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
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
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
    
    if (mShutDown)
        return false;
    if(!gameStart && !gameOver) // Game not started
    {
        pauseLabel->hide();
        mTrayMgr->removeWidgetFromTray(pauseLabel);
        gameOverPanel->hide();
        mTrayMgr->removeWidgetFromTray(gameOverPanel);          
    }
    else if(gameOver)
    {
        gameOverPanel->show();
        mTrayMgr->moveWidgetToTray(gameOverPanel, OgreBites::TL_CENTER);
        gameOverPanel->setParamValue(1, Ogre::StringConverter::toString(score));
        if (gameStart)
            gameOverScreen();
        gameStart = false;
    }
    else // Game started
    {
        if(!gamePause)
        {
            if(!processUnbufferedInput(evt)) 
                return false;
            if (hostPlayer != NULL)
                restrictPlayerMovement(hostPlayer);
            if (clientPlayer != NULL)
                restrictPlayerMovement(clientPlayer);
            gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f); 
            gameSimulator->parseCollisions(); // check collisions
            modifyScore(gameSimulator->tallyScore());
            time_t currTime;
            time(&currTime);
            gameOver = updateTimer(currTime);
            if (gameOver)
                gameMusic->playMusic("Pause");
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
        /*case OIS::KC_RETURN:
            if (!gameStart)
                startGame();
            break;
        case OIS::KC_NUMPADENTER:
            if (!gameStart)
                startGame();
            break;*/
        case OIS::KC_P:
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
bool MCP::startGame(const CEGUI::EventArgs &e)
{
	CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    
    Disk* d = (Disk*)gameSimulator->getGameObject("Disk");
    d->particleNode->setVisible(true);
    gameMusic->playMusic("Play");
    //startLabel->hide();
    //mTrayMgr->removeWidgetFromTray(startLabel);
    objectivePanel->hide();
    instructPanel->hide();
    mTrayMgr->removeWidgetFromTray(instructPanel);
    mTrayMgr->removeWidgetFromTray(objectivePanel);
    gameOverPanel->hide();
    mTrayMgr->removeWidgetFromTray(gameOverPanel);
    gameStart = true;
    gameOver = false;
    score = 0;
    time(&initTime);
    
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::hostGame(const CEGUI::EventArgs &e)
{
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::joinGame(const CEGUI::EventArgs &e)
{
    return true;
}
//-------------------------------------------------------------------------------------
void MCP::togglePause()
{
    if (gamePause == true)  //leaving pause
    {
    	CEGUI::MouseCursor::getSingleton().hide();
    	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    	wmgr.destroyAllWindows();
    	
        gameMusic->playMusic("Play");
        gamePause = false;
        pauseLabel->hide();
        mTrayMgr->removeWidgetFromTray(pauseLabel);
        objectivePanel->hide();
        instructPanel->hide();
        mTrayMgr->removeWidgetFromTray(instructPanel);
        mTrayMgr->removeWidgetFromTray(objectivePanel);
    }
    else //entering Pause
    {
    	CEGUI::MouseCursor::getSingleton().show();
    	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/Pause/Sheet");
    	
    	CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "TronGame/Pause/QuitButton");
    	quit->setText("Quit Game");
    	quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    	
    	sheet->addChildWindow(quit);
    	
    	CEGUI::System::getSingleton().setGUISheet(sheet);
    
    	quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, this));
    	
        gameMusic->playMusic("Start");
        pauseLabel->setCaption("GAME PAUSED!");
        pauseLabel->show();
        mTrayMgr->moveWidgetToTray(pauseLabel, OgreBites::TL_CENTER);
        objectivePanel->show();
        instructPanel->show();
        mTrayMgr->moveWidgetToTray(instructPanel, OgreBites::TL_RIGHT);
        mTrayMgr->moveWidgetToTray(objectivePanel, OgreBites::TL_LEFT);
        gamePause = true;
        time(&pauseTime);
    } 
}
//-------------------------------------------------------------------------------------
void MCP::gameOverScreen() 
{
    CEGUI::MouseCursor::getSingleton().show();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/GameOver/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "TronGame/GameOver/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        
    CEGUI::Window *restart = wmgr.createWindow("TaharezLook/Button", "TronGame/GameOver/RestartGameButton");
    restart->setText("Restart Game");
    restart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    restart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.6, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(restart);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, this));
    restart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::startGame, this));
}
//-------------------------------------------------------------------------------------
bool MCP::createMultiplayerMenu(const CEGUI::EventArgs &e)
{
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/MainMenu/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "TronGame/MultiplayerMenu/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    
    CEGUI::Window *host = wmgr.createWindow("TaharezLook/Button", "TronGame/MultiplayerMenu/HostButton");
    host->setText("Host a game");
    host->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    host->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.45, 0)));
    
    CEGUI::Window *join = wmgr.createWindow("TaharezLook/Button", "TronGame/MainMenu/JoinButton");
    join->setText("Join a game");
    join->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    join->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.56, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(host);
    sheet->addChildWindow(join);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, this));
    host->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::hostGame, this));
    join->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::joinGame, this));
}
//-------------------------------------------------------------------------------------
void MCP::createGUI()
{
    mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    
    CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
    
    CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(0,0));
    
    CEGUI::MouseCursor::getSingleton().show();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/MainMenu/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "TronGame/MainMenu/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        
    CEGUI::Window *singlePlayerStart = wmgr.createWindow("TaharezLook/Button", "TronGame/MainMenu/SinglePlayerStartButton");
    singlePlayerStart->setText("Single Player");
    singlePlayerStart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    singlePlayerStart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.45, 0)));
    
    CEGUI::Window *multiplayerStart = wmgr.createWindow("TaharezLook/Button", "TronGame/MainMenu/MultiplayerStartButton");
    multiplayerStart->setText("Multiplayer");
    multiplayerStart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    multiplayerStart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.56, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(singlePlayerStart);
    sheet->addChildWindow(multiplayerStart);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, this));
    singlePlayerStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::startGame, this));
    multiplayerStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::createMultiplayerMenu, this));
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
    if ((pos.x + dim.x) >= gameRoom->getWidth()/2.0f || (pos.x + dim.x) <= -gameRoom->getWidth()/2.0f)
        p->getBody()->setLinearVelocity(btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), p->getBody()->getLinearVelocity().getZ()));
    // Gap and (Relative) Back wall check
    if ((pos.z + dim.z) >= gameRoom->getHeight()/2.0f || (pos.z + dim.z) <= -gameRoom->getHeight()/2.0f)
        p->getBody()->setLinearVelocity(btVector3(p->getBody()->getLinearVelocity().getX(), p->getBody()->getLinearVelocity().getY(), 0.0f));
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
