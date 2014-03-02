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
    /******************** GAME VARIABLES ********************/
    gamePause = false;
    gameStart = false;
    gameOver = false;
    vKeyDown = false;    
    jumpFlag = false;
    // initialize random number generate
    srand(time(0));

    gameMusic = new Music();
    gameMusic->playMusic("Start");
    
    /********************    SIMULATOR   ********************/
    game_simulator = new Simulator(mSceneMgr, gameMusic);

    /********************    LIGHTS     ********************/
	// Ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Point light
    pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    

    /********************  GAME OBJECTS  ********************/
    PlayerCamera* p1Cam = new PlayerCamera("P1_cam", mSceneMgr, mCamera); // Create PlayerCamera
    game_simulator->setCamera(p1Cam); // Attach camera to game_simulator
    new Room(mSceneMgr, game_simulator); // Create Room
    (new Disk("Disk", mSceneMgr, game_simulator, Ogre::Math::RangeRandom(0,1)))->addToSimulator(); // Create Disk
   
    (new Player("Player1", mSceneMgr, game_simulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(1.0f, 1.0f, 1.0f)))->addToSimulator(); // Create Player 1
    (new Target("Target1", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target2", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    (new Target("Target3", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator(); // Create initial Target
    gameDisk = (Disk*)game_simulator->getGameObject("Disk"); // Attach Disk to game_simulator
    pointLight->setPosition(Ogre::Vector3(0.0f, game_simulator->getGameObject("Ceiling")->getSceneNode()->getPosition().y, 0.0f));

    /********************    OVERLAYS    ********************/
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

    p1Cam->setCHOverlays(crossHairVertOverlay, crossHairHorizOverlay); // WDTD
}

//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static Ogre::Real mMove = 3.0f;                                    // The movement constant
    static Ogre::Real jumpMove = 8.0f;
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    float fx = 0.0f;                                                   // Force x-component
    float fy = 0.0f;                                                   // Force y-component
    float fz = 0.0f;                                                   // Force z- component
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse
    
    float sprintFactor = 1.0f;                                         // How fast the character moves when Left Shift is held down
 
    Player *p = (Player *)game_simulator->getGameObject("Player1");    // Get the player object from the simulator
    
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);            // Initial velocity vector
    btVector3 jumpVector = btVector3(0.0f, 0.0f, 0.0f);

    /********* START THE GAME *********/
    if ((mKeyboard->isKeyDown(OIS::KC_RETURN) || mKeyboard->isKeyDown(OIS::KC_NUMPADENTER)) && !gameStart)
    {
        Disk* d = (Disk*)game_simulator->getGameObject("Disk");
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

    /******************** PAUSE THE GAME ********************/
    if (mKeyboard->isKeyDown(OIS::KC_P) && !pausePressedLast)
    {
        if (gamePause == true)  //leaving pause
        {
            gameMusic->playMusic("Play");
            pausePressedLast = true;
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
            pausePressedLast = true;
            gamePause = true;
            time(&pauseTime);
        } 
    }
    else if (!mKeyboard->isKeyDown(OIS::KC_P))
    {
        pausePressedLast = false;
    }

    /********************     MOVEMENT   ********************/
    // Allow movement if the player is on the floor and the game is not paused
    if(game_simulator->checkOnFloor()  && !gamePause)
    {
        // If the mouse button was not pressed in the last frame, the mouse is pressed in the current frame, and the player is holding the disk then they are trying to throw
        if(!mMouseDown && currMouse && p->checkHolding() && vKeyDown) 
        {
            gameMusic->playMusic("Throw");
            game_simulator->setThrowFlag();
            p->getPlayerDisk()->getSceneNode()->setVisible(true, true);
        }
        mMouseDown = currMouse; // Set that the mouse WAS pressed
        
        // Move into aiming-mode
        // if 'v' is pressed and was not pressed last frame - go to aim mode
        if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeyDown)
        {
            PlayerCamera* pc = game_simulator->getPlayerCamera("P1_cam");
            game_simulator->toggleViewChange("Player1");
            pc->toggleThirdPersonView();
            vKeyDown = true;
        }
	    
        // if 'v' is not pressed and was pressed last frame - exit aim mode
        if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown)
        {
            PlayerCamera* pc = game_simulator->getPlayerCamera("P1_cam");
            game_simulator->toggleViewChange("Player1");
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
            if (mKeyboard->isKeyDown(OIS::KC_SPACE)) // Don't this Spacebar make my people wanna jump
            {
                gameMusic->playMusic("Jump");
                fy += jumpMove; // Jump, Jump
                jumpVector = jumpVector + btVector3(0.0f, fy, 0.0f);
                keyWasPressed = true;
                jumpFlag = true;
                game_simulator->resetOnFloor();
            }
            if(keyWasPressed == true && !vKeyDown)
                p->getBody()->setLinearVelocity((velocityVector * sprintFactor) + jumpVector);

        }
    }
    return true;
}

bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    Player* p = (Player*)game_simulator->getGameObject("Player1");
    PlayerCamera* pcam = game_simulator->getPlayerCamera("P1_cam");

    // if 'v' is pressed and was pressed last frame - still in aim mode
    if (vKeyDown)
    {   
        // Set bounds are not working
        // if ((Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getRoll()) > Ogre::Degree(-85) 
        //     && (Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getRoll()) < Ogre::Degree(85))
            p->getPlayerSightNode()->translate(evt.state.X.rel/25.0f, 0.0f, 0.0f);
        // if ((Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getPitch()) > Ogre::Degree(-85) 
        //     && (Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getPitch()) < Ogre::Degree(85))
            p->getPlayerSightNode()->translate(0.0f, -evt.state.Y.rel/25.0f, 0.0f);
    }
    else
    {
    //  if ((Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getRoll()) > Ogre::Degree(-85) 
    //        && (Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getRoll()) < Ogre::Degree(85))
            p->getPlayerSightNode()->translate(evt.state.X.rel/5.0f, 0.0f, 0.0f);
    //  if (((Ogre::Degree)pcam->getPCamSceneNode()->getOrientation().getPitch()) > Ogre::Degree(-85) 
    //        && (Ogre::Degree)(pcam->getPCamSceneNode()->getOrientation().getPitch()) < Ogre::Degree(85))
            p->getPlayerSightNode()->translate(0.0f, -evt.state.Y.rel/5.0f, 0.0f);
    }
}


//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
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
            game_simulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f); 
            game_simulator->setHitFlags(); // check collisions
            modifyScore(game_simulator->tallyScore());
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

    if(!processUnbufferedInput(evt)) 
        return false;
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
