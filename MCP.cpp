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
    gameServer = NULL;
    mainClient = NULL;
    solo = NULL;  

    gameMusic = new Music();    // Initialize Music
    // gameMusic->playMusic("Start");

    gui = new GUI(this); 
    gui->createMainMenu(); 
}
//-------------------------------------------------------------------------------------
void MCP::createSoloModeScene()
{
    
}
//-------------------------------------------------------------------------------------
bool MCP::soloMode(const CEGUI::EventArgs &e)   // Need to make a soloMode class possibly
{
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);

    // time(&initTime);     

    gui->addPanel(scorePanel, OgreBites::TL_BOTTOMRIGHT);

    gameMusic->playMusic("Play");
    
    // createSoloModeScene();
    solo = new Solo(this);    
    
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::hostGame(const CEGUI::EventArgs &e)
{
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();

    // gameMusic->playMusic("Play");

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);
    gui->removePanel(powerUpPanel);
    
    // Create Server Object
    gameServer = new Server(this);//gameMusic, mSceneMgr);
    
    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::joinGame(const CEGUI::EventArgs &e)
{   
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::WindowManager::WindowIterator wIterator = wmgr.getIterator();  
    wIterator++;

    std::string ip_string = wIterator.getCurrentValue()->getText().c_str();
    char* ip = new char[ip_string.length()+1];
    std::memcpy(ip, wIterator.getCurrentValue()->getText().c_str(), ip_string.length()+1);

    CEGUI::MouseCursor::getSingleton().hide();
    wmgr.destroyAllWindows();

    // gameMusic->playMusic("Play");

    gui->removePanel(objectivePanel);
    gui->removePanel(instructPanel);
    gui->removePanel(powerUpPanel);

    // Create Client Object
    mainClient = new Client(ip, mSceneMgr);

    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
    
    if(!processUnbufferedInput(evt)) 
        return false;

    if (mShutDown)
        return false;

    if (gameServer != NULL)
        gameServer->frameRenderingQueued(evt);

    if (mainClient != NULL)
        mainClient->frameRenderingQueued(evt, mKeyboard, mMouse);

    if(solo != NULL)
        solo->frameRenderingQueued(evt);

    /* Call Client and Server frameRenderingQueued */
    
    // static int count = 0;
    
    // if (hostPlayer != NULL && checkGameLoss(hostPlayer)) {
    //     player1Loss = true;
    //     gameOver = true;
    // }
        
    // if (clientPlayer != NULL && checkGameLoss(clientPlayer)) {
    //     player2Loss = true;
    //     gameOver = true;
    // }

    // if (gameMode == 1) // attempt to establish the connection
    // {
    //     if (!gameNetwork->checkConnection())
    //     {
    //         if (!gameNetwork->establishConnection())
    //             return true;
    //         else 
    //             gameStart = true;
    //     }
    //     else if (!gameStart)
    //         gameStart = true;
    // }
    // if(!gameStart && !gameOver) // Game not started
    // {
    //     gui->removeLabel(pauseLabel);
    //     gui->removePanel(gameOverPanel);
    //     gui->removePanel(gameOverWinPanel);
    //     gui->removePanel(gameOverLossPanel); 
    // }
    // else if(gameOver)
    // {
    //     if (gameMode == 1) {
    //         if (player1Loss) {
    //             if (clientServerIdentifier == 0) {
    //                 gui->addPanel(gameOverLossPanel, OgreBites::TL_CENTER);
    //             }
    //             else {
    //                 gui->addPanel(gameOverWinPanel, OgreBites::TL_CENTER);
    //             }
    //         }
    //         else if (player2Loss) {
    //             if (clientServerIdentifier == 0) {
    //                 gui->addPanel(gameOverWinPanel, OgreBites::TL_CENTER);
    //             }
    //             else {
    //                 gui->addPanel(gameOverLossPanel, OgreBites::TL_CENTER);
    //             }
    //         }
    //     }
    //     else {
    //         gui->addPanel(gameOverPanel, OgreBites::TL_CENTER);
    //         gameOverPanel->setParamValue(1, Ogre::StringConverter::toString(score));
    //     }
    //     if (gameStart && clientServerIdentifier == 0)
    //         gui->gameOverScreen();
    //     else if (gameStart && clientServerIdentifier == 1)
    //         gui->otherGameOverScreen();
    //     gameStart = false;
    // }
    // else // Game started
    // {
    //     if(!gamePause)
    //     {
    //         if(gameMode == 0)    // Limit Solo mode to option of pausing
    //         {
    //             time_t currTime;
    //             time(&currTime);
    //             gameOver = updateTimer(currTime);
    //             if (gameOver)
    //                 gameMusic->playMusic("Pause");
    //             modifyScore(gameSimulator->tallyScore());
    //         }
    //         if (clientServerIdentifier == 0)     // Host render loop - Specific processing of inputs
    //         {
    //             renderLoop_Host(evt);
    //         }
    //         else if (clientServerIdentifier == 1)    // Client render loop - Specific processing of inputs
    //         {
    //             renderLoop_Client(evt);
    //         }     
    //     }
    //     else
    //     {
    //                 time_t pcurrTime;
    //                 time(&pcurrTime);
    //                 updatePauseTime(pcurrTime);
    //     }
    // }
    return ret;
}
//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{

    if(solo != NULL)
        solo->processUnbufferedInput(evt, mKeyboard, mMouse);
    /********************  KEY VARIABLES ********************/    
    // static bool mMouseDown = false;                                    // If a mouse button is depressed
    // static bool pausePressedLast = false;                              // Was pause pressed last frame
    // static bool spacePressedLast = false;
    // static Ogre::Real timeSinceLastJump = 0.0f;
    // bool turboMode = false;
    // bool keyWasPressed = false;                                        // Was a key pressed in current frame
    // bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    // Player *p = (Player *)gameSimulator->getGameObject("Player1");     // Get the player object from the simulator

    // float fx = 0.0f;                                                   // Force x-component
    // float fz = 0.0f;                                                   // Force z-component
    // btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);            // Initial velocity vector

    // timeSinceLastJump += evt.timeSinceLastFrame;

    // /********************     MOVEMENT   ********************/
    // // Allow movement if the player is on the floor and the game is not paused
    // if(!gamePause && gameSimulator->checkGameStart())
    // {
    //     // If the mouse button was not pressed in the last frame, the mouse is pressed in the current frame, and the player is holding the disk then they are trying to throw
    //     if(!mMouseDown && currMouse && p->checkHolding() && vKeyDown) 
    //     {
    //         gameMusic->playMusic("Throw");
    //         gameSimulator->setThrowFlag();
    //         p->getPlayerDisk()->getSceneNode()->setVisible(true, true);
    //     }
    //     mMouseDown = currMouse; // Set that the mouse WAS pressed
        
    //     if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeyDown) // if 'v' is pressed and was not pressed last frame - go to aim mode
    //     {
    //         PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
    //         gameSimulator->toggleViewChange("Player1");
    //         pc->toggleThirdPersonView();
    //         vKeyDown = true;
    //         //showTrajectory(pc);
    //     }
    //     if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown) // if 'v' is not pressed and was pressed last frame - exit aim mode
    //     {
    //         PlayerCamera* pc = gameSimulator->getPlayerCamera("P1Cam");
    //         gameSimulator->toggleViewChange("Player1");
    //         pc->toggleThirdPersonView();
    //         vKeyDown = false;
    //         //mSceneMgr->getRootSceneNode()->detachObject(trajectory);
    //         //trajectory->clear();
    //     }
    //     if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)) // Move into Boost mode
    //     {
    //         turboMode = true;
    //     }
    //     // If the 'V' key is down you shouldn't be able to move
    //     if (!vKeyDown)  
    //     {
    //          // Move the player
    //         if (mKeyboard->isKeyDown(OIS::KC_W)) // Forward
    //         {
    //             fz -= mMove;
    //             velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
    //             keyWasPressed = true;
    //         }
    //         if (mKeyboard->isKeyDown(OIS::KC_S)) // Backward
    //         {
    //             fz += mMove;
    //             velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
    //             keyWasPressed = true;
    //         }

    //         if (mKeyboard->isKeyDown(OIS::KC_A)) // Left - yaw or strafe
    //         {
    //             fx -= mMove; // Strafe left
    //             velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
    //             keyWasPressed = true;
    //         }
    //         if (mKeyboard->isKeyDown(OIS::KC_D)) // Right - yaw or strafe
    //         {
    //             fx += mMove; // Strafe right
    //             velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
    //             keyWasPressed = true;
    //         }
    //         if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !p->groundConstantSet && !spacePressedLast) 
    //         {
    //             if(p->performJump())
    //             {
    //                 if (p->jumpPowerActive)
    //                     gameMusic->playMusic("SuperJump");
    //                 else
    //                     gameMusic->playMusic("Jump");
    //                 spacePressedLast = true;
    //                 gameSimulator->soundedJump = true;
    //             }
    //         }
    //         if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && spacePressedLast)
    //             spacePressedLast = false;
    //         if(keyWasPressed && !p->checkMovementRestriction())
    //         {   // Rotate the velocity vector by the orientation of the player
    //             Ogre::Vector3 trueVelocity = Ogre::Vector3(velocityVector.getX(), velocityVector.getY(), velocityVector.getZ());
    //             trueVelocity = p->getSceneNode()->getOrientation() * trueVelocity; 
    //             btVector3 btTrueVelocity = btVector3(trueVelocity.x, trueVelocity.y, trueVelocity.z);

    //             if (turboMode)
    //                     p->getBody()->setLinearVelocity((btTrueVelocity * sprintFactor) + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
    //             else
    //                     p->getBody()->setLinearVelocity(btTrueVelocity + btVector3(0.0f, p->getBody()->getLinearVelocity().getY(), 0.0f));
    //         }
    //     }
    // }
    return true;
}

bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    // Scroll wheel.
    if (evt.state.Z.rel)
        sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
    if(solo != NULL)
        solo->mouseMoved(evt);

    if (mainClient != NULL)
        mainClient->mouseMoved(evt.state.X.rel, evt.state.Y.rel);

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
            // if (gameMode == 0)
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
    // gui->pauseMenu(gamePause);
    
    // if (gamePause == true)  //leaving pause
    // {        
    //     gameMusic->playMusic("Play");
    //     gamePause = false;
    //     gui->removeLabel(pauseLabel);
    //     gui->removePanel(objectivePanel);
    //     gui->removePanel(instructPanel);
    // }
    // else //entering Pause
    // {        
    //     gameMusic->playMusic("Start");
    //     pauseLabel->setCaption("GAME PAUSED!");
    //     gui->addLabel(pauseLabel, OgreBites::TL_CENTER);
    //     gui->addPanel(objectivePanel, OgreBites::TL_TOP);
    //     gui->addPanel(instructPanel, OgreBites::TL_RIGHT);
    //     gamePause = true;
    //     time(&pauseTime);
    // }
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
void MCP::createOverlays(PlayerCamera* playCam) // might move to Client and Server
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
void MCP::setShutDown()
{
    mShutDown = true;
}
//-------------------------------------------------------------------------------------
Ogre::SceneManager* MCP::getSceneManager()
{
    return mSceneMgr;
}  
//-------------------------------------------------------------------------------------
Music* MCP::getMusicObject()
{
    return gameMusic;
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