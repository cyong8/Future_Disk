#include "Solo.h"

Solo::Solo(MCP* mcp)//Music* mus, Ogre::SceneManager* mgr)
{
	MasterControl = mcp;
	gameMusic = mcp->getMusicObject();
	sceneMgr = mcp->getSceneManager();

	gameSimulator = new Simulator(sceneMgr, gameMusic);

    srand(time(0));
    time(&gapStartTime);
    time(&gapEndTime);

	timeSinceLastStateUpdate = 0.01f;
    mMove = 5.0f;
    mRotate = 0.1f;
    sprintFactor = 2.0f;
    gameDisk = NULL;
    gameStart = false;
    gameOver = false;
    gamePause = false;

    createScene();
}
//-------------------------------------------------------------------------------------
Solo::~Solo(void)
{
}
//-------------------------------------------------------------------------------------
void Solo::createScene()
{
    gameRoom = new Room(sceneMgr, gameSimulator, 0);

    player = new Player("Player1", sceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), 1);
    player->addToSimulator();
    
    pCam = new PlayerCamera("soloCamera", sceneMgr, sceneMgr->getCamera("PlayerCam"));/*need camera object*/
    pCam->initializePosition(player->getPlayerCameraNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(player);

    // These actually don't belong - initialize target list instead
    /*
    Power = new Target("Power", sceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Speed = new Target("Speed", sceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    JumpPower = new Target("Jump", sceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Restore = new Target("Restore", sceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());

    Power->addToSimulator();
    Speed->addToSimulator();
    JumpPower->addToSimulator();
    Restore->addToSimulator();
    */

    // Lights
    sceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = sceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getWall(Ceiling)->getSceneNode()->getPosition().y, 0.0f));

    gameStart = true;

    createOverlays(pCam); // in MCP
}
//-------------------------------------------------------------------------------------
bool Solo::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    // else if (!gameStart) // may need
    //     gameStart = true;
    //bool ret = BaseApplication::frameRenderingQueued(evt);

    gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f);
    gameSimulator->parseCollisions(); // check collisions


    if(!gameStart && !gameOver) // Game not started
    {
        MasterControl->gui->removeLabel(MasterControl->getLabel(PAUSE));
        MasterControl->gui->removePanel(MasterControl->getPanel(GAMEOVER));
        printf("!gamestart && !gameOver");
    }
    else if(gameOver)
    {
        //gui->addPanel(gameOverPanel, OgreBites::TL_CENTER);
        //gameOverPanel->setParamValue(1, Ogre::StringConverter::toString(score));
        printf("gameOver");
        gameStart = false;

        // Add option to restart game
    }
    else // Game started
    {
        if(!gamePause)
        {
            time_t currTime;
            time(&currTime);
            //gameOver = updateTimer(currTime);
            if (gameOver)
                gameMusic->playMusic("Pause");
            //modifyScore(gameSimulator->tallyScore());
        }
        else
        {
            time_t pcurrTime;
            time(&pcurrTime);
            //updatePauseTime(pcurrTime);
        }
    }

    //restrictPlayerMovement(player);

    if (gameSimulator->setDisk && gameSimulator->gameDisk == NULL)
    {
        gameDisk = new Disk("Disk", sceneMgr, gameSimulator, 0.0f/*Ogre::Math::RangeRandom(0,2)*/);
        gameDisk->addToSimulator();
    }

    if (timeSinceLastStateUpdate < 0.0f)
        timeSinceLastStateUpdate = 0.01f;

    updateCamera(evt.timeSinceLastFrame);

    //timeSinceLastStateUpdate -= evt.timeSinceLastFrame;
}
//-------------------------------------------------------------------------------------
void Solo::updateCamera(Ogre::Real elapseTime)
{
    if (pCam->isInAimMode())
        pCam->update(elapseTime, player->getSceneNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());
    else
        pCam->update(elapseTime, player->getPlayerCameraNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());      
}
//-------------------------------------------------------------------------------------
bool Solo::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    // Scroll wheel.
    if (evt.state.Z.rel)
        sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);

    Ogre::SceneNode* pSceneNode = player->getSceneNode();
    Ogre::SceneNode* pSightNode = player->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = player->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;

    if (pCam->isInAimMode())
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate /2) * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        //clientOrientationChange = true;
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * evt.state.X.rel), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -evt.state.Y.rel, 0.0f);
        //clientOrientationChange = true;
    }
    pSightNode->setPosition(pSightNode->getPosition() + sightHeight);
    pCamNode->setPosition(pCamNode->getPosition().x, pCamNode->getPosition().y, -pSightNode->getPosition().z);

    return true;
}
bool Solo::processUnbufferedInput(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    static bool spacePressedLast = false;
    static Ogre::Real timeSinceLastJump = 0.0f;
    bool turboMode = false;
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    static bool vKeyDown = false;

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
            //PlayerCamera* pc = gameSimulator->getPlayerCamera("soloCamera");
            // gameSimulator->toggleViewChange("Player1");
            pCam->toggleThirdPersonView();
            vKeyDown = true;
            //showTrajectory(pc);
        }
        if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown) // if 'v' is not pressed and was pressed last frame - exit aim mode
        {
            //PlayerCamera* pc = gameSimulator->getPlayerCamera("soloCamera");
            // gameSimulator->toggleViewChange("Player1");
            pCam->toggleThirdPersonView();
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
            if(keyWasPressed && !p->checkMovementRestriction())
            {   // Rotate the velocity vector by the orientation of the player
                
                Ogre::Vector3 trueVelocity = Ogre::Vector3(velocityVector.getX(), velocityVector.getY(), velocityVector.getZ());
                trueVelocity = p->getSceneNode()->_getDerivedOrientation() * trueVelocity; 
                btVector3 btTrueVelocity = btVector3(trueVelocity.x, trueVelocity.y, trueVelocity.z);

                if (turboMode)
                        p->getBody()->setLinearVelocity((btTrueVelocity * sprintFactor) + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
                else
                        p->getBody()->setLinearVelocity(btTrueVelocity + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
            }
        }
    }
}

void Solo::createOverlays(PlayerCamera* playCam) // might move to Client and Server
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