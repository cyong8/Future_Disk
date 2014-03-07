#include "MCP.h"

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
    // Initialize random number generate
    srand(time(0));

    /******************** GAME STATE FLAGS ********************/
    gamePause = false;
    gameStart = false;
    gameOver = false;
    vKeyDown = false;    
    
    gameMusic = new Music();    // Initialize Music
    gameMusic->playMusic("Start");
    
    gameSimulator = new Simulator(mSceneMgr, gameMusic);   // Initialize Simulator

    /********************  OBJECT CREATION/ASSIGNMENT  ********************/
    PlayerCamera* p1Cam = new PlayerCamera("P1Cam", mSceneMgr, mCamera); 
    gameSimulator->setCamera(p1Cam); 
    new Room(mSceneMgr, gameSimulator); 
    (new Disk("Disk", mSceneMgr, gameSimulator, Ogre::Math::RangeRandom(0,1)))->addToSimulator(); 
    (new Player("Player1", mSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(1.0f, 1.0f, 1.0f)))->addToSimulator(); // Create Player 1
    (new Target("Target1", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target2", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target3", mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    gameDisk = (Disk*)gameSimulator->getGameObject("Disk");

    /********************    LIGHTS     ********************/
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = mSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameSimulator->getGameObject("Ceiling")->getSceneNode()->getPosition().y, 0.0f));

    createOverlays(p1Cam);
}

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
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static Ogre::Real mMove = 3.0f;                                    // The movement constant
    static Ogre::Real jumpMove = 8.0f;
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    static bool spacePressedLast = false;
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    Player *p = (Player *)gameSimulator->getGameObject("Player1");    // Get the player object from the simulator

    float fx = 0.0f;                                                   // Force x-component
    float fy = 0.0f;                                                   // Force y-component
    float fz = 0.0f;                                                   // Force z- component
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);            // Initial velocity vector
    
    float sprintFactor = 1.0f;                                         // How fast the character moves when Left Shift is held down
    btVector3 jumpVector = btVector3(0.0f, 0.0f, 0.0f);
    
    /********* START THE GAME *********/
    if ((mKeyboard->isKeyDown(OIS::KC_RETURN) || mKeyboard->isKeyDown(OIS::KC_NUMPADENTER)) && !gameStart)
        startGame();

    /******************** PAUSE THE GAME ********************/
    if (mKeyboard->isKeyDown(OIS::KC_P) && !pausePressedLast)
    {
        togglePause();
        pausePressedLast = true;
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_P))
        pausePressedLast = false;
    

    /********************     MOVEMENT   ********************/
    // Allow movement if the player is on the floor and the game is not paused
    if(!gamePause)
    {
        // If the mouse button was not pressed in the last frame, the mouse is pressed in the current frame, and the player is holding the disk then they are trying to throw
        if(!mMouseDown && currMouse && p->checkHolding() && vKeyDown) 
        {
            gameMusic->playMusic("Throw");
            gameSimulator->setThrowFlag();
            p->getPlayerDisk()->getSceneNode()->setVisible(true, true);
        }
        mMouseDown = currMouse; // Set that the mouse WAS pressed
        
        // Move into aiming-mode
        // if 'v' is pressed and was not pressed last frame - go to aim mode
        if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeyDown)
        {
            PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
            gameSimulator->toggleViewChange("Player1");
            pc->toggleThirdPersonView();
            vKeyDown = true;
        }
	    
        // if 'v' is not pressed and was pressed last frame - exit aim mode
        if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown)
        {
            PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
            gameSimulator->toggleViewChange("Player1");
            pc->toggleThirdPersonView();
            vKeyDown = false;
        }

        // Move into Boost mode
        if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)) 
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
            if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !spacePressedLast &&
                    (p->getSceneNode()->getPosition().y >= p->getGroundY())) 
            {
                gameMusic->playMusic("Jump");
                fy += jumpMove; 
                jumpVector = jumpVector + btVector3(0.0f, fy, 0.0f);
                keyWasPressed = true;
                gameSimulator->soundedJump = true;
                spacePressedLast = true;
                p->jumping = true;
            }
            if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && spacePressedLast)
                spacePressedLast = false;

            if(keyWasPressed == true)
                p->getBody()->setLinearVelocity((velocityVector * sprintFactor) + jumpVector + (btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f)));
        }
    }
    return true;
}

bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    Player* p = (Player*)gameSimulator->getGameObject("Player1");
    PlayerCamera* pcam = gameSimulator->getPlayerCamera("P1Cam");

    // if 'v' is pressed and was pressed last frame - still in aim mode
    // Need to add translation bounds
    if (vKeyDown)
    {   
        mCamNode->yaw(Ogre::Degree(-mRotate * arg.state.X.rel), Ogre::Node::TS_WORLD);
        mCamNode->pitch(Ogre::Degree(-mRotate * arg.state.Y.rel), Ogre::Node::TS_LOCAL);
    }
    // if (vKeyDown)
    // {   
    //         p->getPlayerSightNode()->translate(evt.state.X.rel/25.0f, 0.0f, 0.0f);
    //         p->getPlayerSightNode()->translate(0.0f, -evt.state.Y.rel/25.0f, 0.0f);
    // }
    // else
    // {
    //         p->getPlayerSightNode()->translate(evt.state.X.rel/5.0f, 0.0f, 0.0f);
    //         p->getPlayerSightNode()->translate(0.0f, -evt.state.Y.rel/5.0f, 0.0f);
    // }
}


//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
    
    if(!processUnbufferedInput(evt)) 
        return false;


    if(!gameStart && !gameOver) // Game not started
    {
        pauseLabel->hide();
        mTrayMgr->removeWidgetFromTray(pauseLabel);
        gameOverPanel->hide();
        mTrayMgr->removeWidgetFromTray(gameOverPanel);
        startLabel->show();
        startLabel->setCaption("Press ENTER to begin!");
    }
    else if(gameOver)
    {
        gameOverPanel->show();
        mTrayMgr->moveWidgetToTray(gameOverPanel, OgreBites::TL_CENTER);
        gameOverPanel->setParamValue(1, Ogre::StringConverter::toString(score));
        gameStart = false;
    }
    else // Game started
    {
        if(!gamePause)
        {
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

bool MCP::keyPressed(const OIS::KeyEvent &evt)
{
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
    }
    return true;
}
void MCP::startGame()
{
    Disk* d = (Disk*)gameSimulator->getGameObject("Disk");
    d->particleNode->setVisible(true);
    gameMusic->playMusic("Play");
    startLabel->hide();
    mTrayMgr->removeWidgetFromTray(startLabel);
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
}
void MCP::togglePause()
{
    if (gamePause == true)  //leaving pause
    {
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