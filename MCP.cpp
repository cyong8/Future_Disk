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
bool MCP::soloMode(const CEGUI::EventArgs &e)
{
    CEGUI::MouseCursor::getSingleton().hide();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();

    // gui->removePanel(objectivePanel);
    //gui->removePanel(instructPanel);

    gui->addPanel(scorePanel, OgreBites::TL_BOTTOMRIGHT);

    gameMusic->playMusic("Play");

    if (solo != NULL)
        solo->restartGame();
    else
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

    // gui->removePanel(objectivePanel);
    //gui->removePanel(instructPanel);
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

    // gui->removePanel(objectivePanel);
    //gui->removePanel(instructPanel);
    gui->removePanel(powerUpPanel);

    // Create Client Object
    mainClient = new Client(ip, mSceneMgr);

    return true;
}
//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);    

    if (mShutDown)
        return false;

    if (gameServer != NULL)
        gameServer->frameRenderingQueued(evt.timeSinceLastFrame);

    if (mainClient != NULL)
        mainClient->frameRenderingQueued(evt.timeSinceLastFrame, mKeyboard, mMouse);

    if(solo != NULL)
        solo->frameRenderingQueued(evt.timeSinceLastFrame, mKeyboard, mMouse);

    if(!processUnbufferedInput(evt)) 
        return false;

    return ret;
}
//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    return true;
}

bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    CEGUI::System &sys = CEGUI::System::getSingleton();
    sys.injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
    // Scroll wheel.
    if (evt.state.Z.rel)
        sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);

    if (solo != NULL)
        solo->mouseMoved(evt.state.X.rel, evt.state.Y.rel);

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
            if (solo != NULL)
                solo->togglePause();
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
bool MCP::checkGameLoss(Player* p) {
    Ogre::Vector3 pos = p->getSceneNode()->getPosition();
    
    if (pos.y < -40.0f)
        return true;
    return false;
}
//-----------------------------------------------------------------------------------
bool MCP::activateMainMenu(const CEGUI::EventArgs &e) 
{   
    gui->createMainMenu();
    return true;
}
//-----------------------------------------------------------------------------------
bool MCP::activateMainMenuSolo(const CEGUI::EventArgs &e) 
{   
    gui->removePanel(scorePanel);
    gui->removePanel(gameOverPanel);
    gui->removeLabel(pauseLabel);
    
    //gui->addPanel(instructPanel, OgreBites::TL_RIGHT);
    //gui->addPanel(objectivePanel, OgreBites::TL_BOTTOM);
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
