/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "BaseApplication.h"

//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
    if (mTrayMgr) delete mTrayMgr;

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "TutorialApplication Render Window");

        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam"); 
    mCamera->setNearClipDistance(.1);
    mCamera->setPosition(Ogre::Vector3(0.0f, 85.0f, 65.0f)); 
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    // create FPS tracker
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mMouse, this);
    mTrayMgr->hideCursor();

    /************************************************************/
    /*************** create timer and score table ***************/
    /************************************************************/
    // create paramsPanel
    Ogre::StringVector scores;
    scores.push_back("Score ");
    scores.push_back("Time ");
    scorePanel = mTrayMgr->createParamsPanel(OgreBites::TL_BOTTOMRIGHT, "ScorePanel", 200, scores);
    score = 0;
    scorePanel->setParamValue(0, Ogre::StringConverter::toString(score));
    scorePanel->setParamValue(1, Ogre::StringConverter::toString(2) + ":00");
    scorePanel->hide();
    mTrayMgr->removeWidgetFromTray(scorePanel);

    //startLabel = mTrayMgr->createLabel(OgreBites::TL_CENTER, "Start", "", 350);

    // Ogre::StringVector objectiveDetails;
    // objectiveDetails.push_back("Modes of Play");
    // objectiveDetails.push_back("Single Player");
    // objectiveDetails.push_back("Multiplayer");
    // objectivePanel = mTrayMgr->createParamsPanel(OgreBites::TL_BOTTOM, "objectivePanel", 650, objectiveDetails); 
    // objectivePanel->setParamValue(0, "");   
    // objectivePanel->setParamValue(1, "Time Attack! Break the targets!");
    // objectivePanel->setParamValue(2, "Knock your opponent down into the abyss by ricocheting disks off walls!");
                                    /* "Indirectly hit tiles in order to remove them and force your opponent into the void" */

    /*Ogre::StringVector instructions;
    instructions.push_back("GAME CONTROLS");
    instructions.push_back("Move Forward ");
    instructions.push_back("Move Backward ");
    instructions.push_back("Strafe Left ");
    instructions.push_back("Strafe Right ");
    instructions.push_back("Jump ");
    instructions.push_back("Boosters ");
    instructions.push_back("Enter Aim View ");
    instructions.push_back("Shoot ball! (While In Aim View) ");
    instructions.push_back("Pause/Unpause Game ");
    instructions.push_back("Toggle Sound ");
    instructPanel = mTrayMgr->createParamsPanel(OgreBites::TL_RIGHT, "instructPanel", 400, instructions);
    instructPanel->setParamValue(0, "");
    instructPanel->setParamValue(1, "W");
    instructPanel->setParamValue(2, "S");
    instructPanel->setParamValue(3, "A");
    instructPanel->setParamValue(4, "D");
    instructPanel->setParamValue(5, "Space Bar");
    instructPanel->setParamValue(6, "Hold Left Shift");
    instructPanel->setParamValue(7, "Hold V");
    instructPanel->setParamValue(8, "Left Mouse Button");
    instructPanel->setParamValue(9, "P");
    instructPanel->setParamValue(10, "M");*/

    pauseLabel = mTrayMgr->createLabel(OgreBites::TL_CENTER, "Pause", "", 155);
    pauseLabel->hide();
    mTrayMgr->removeWidgetFromTray(pauseLabel);

    Ogre::StringVector gOverText;
    gOverText.push_back("GAME OVER!");
    gOverText.push_back("Your Score");
    gameOverPanel = mTrayMgr->createParamsPanel(OgreBites::TL_CENTER, "GameOver", 400, gOverText);
    gameOverPanel->setParamValue(0, "");
    gameOverPanel->setParamValue(1, "");
    gameOverPanel->hide();
    mTrayMgr->removeWidgetFromTray(gameOverPanel);
    
    Ogre::StringVector gOverTextWin; 
    gOverTextWin.push_back("GAME OVER");
    gameOverWinPanel = mTrayMgr->createParamsPanel(OgreBites::TL_CENTER, "GameOverWin", 400, gOverTextWin);
    gameOverWinPanel->setParamValue(0, "YOU WIN!");
    gameOverWinPanel->hide();
    mTrayMgr->removeWidgetFromTray(gameOverWinPanel);
    Ogre::StringVector gOverTextLoss;
    
    gOverTextLoss.push_back("GAME OVER");
    gameOverLossPanel = mTrayMgr->createParamsPanel(OgreBites::TL_CENTER, "GameOverLose", 400, gOverTextLoss);
    gameOverLossPanel->setParamValue(0, "YOU LOSE...");
    gameOverLossPanel->hide();
    mTrayMgr->removeWidgetFromTray(gameOverLossPanel);

    //mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    
    Ogre::StringVector powerUps;
    powerUps.push_back("MULTIPLAYER POWER-UPS");
    powerUps.push_back("Red Target");
    powerUps.push_back("Green Target");
    powerUps.push_back("Cyan Target");
    powerUps.push_back("White Target");
    powerUpPanel = mTrayMgr->createParamsPanel(OgreBites::TL_BOTTOMLEFT, "powerUpPanel", 500, powerUps);
    powerUpPanel->setParamValue(0, "");
    powerUpPanel->setParamValue(1, "Shots knock out more tiles at once!");
    powerUpPanel->setParamValue(2, "Disk doubles its speed!");
    powerUpPanel->setParamValue(3, "Jump higher!");
    powerUpPanel->setParamValue(4, "Restore one of your tiles!");
    powerUpPanel->hide();
    mTrayMgr->removeWidgetFromTray(powerUpPanel);
    
    mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
                archName = Ogre::String(Ogre::macBundlePath() + "/Contents/Resources/" + archName);
#endif
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    Ogre::String workingDir = Ogre::macBundlePath()+"/Contents/Resources/";
    mResourcesCfg = workingDir + mResourcesCfg;
    mPluginsCfg = workingDir + mPluginsCfg;
#endif

    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    mRoot = new Ogre::Root(mPluginsCfg);

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

    // Create the scene
    createScene();

    createFrameListener();

    return true;
};
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    mTrayMgr->frameRenderingQueued(evt);

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

    if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
        mTrayMgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
        if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            mDetailsPanel->show();
        }
        else
        {
            mTrayMgr->removeWidgetFromTray(mDetailsPanel);
            mDetailsPanel->hide();
        }
    }
    else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::TextureFilterOptions tfo;
        unsigned int aniso;

        switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
            newVal = "Trilinear";
            tfo = Ogre::TFO_TRILINEAR;
            aniso = 1;
            break;
        case 'T':
            newVal = "Anisotropic";
            tfo = Ogre::TFO_ANISOTROPIC;
            aniso = 8;
            break;
        case 'A':
            newVal = "None";
            tfo = Ogre::TFO_NONE;
            aniso = 1;
            break;
        default:
            newVal = "Bilinear";
            tfo = Ogre::TFO_BILINEAR;
            aniso = 1;
        }

        Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
        Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
        mDetailsPanel->setParamValue(9, newVal);
    }
    else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
        Ogre::String newVal;
        Ogre::PolygonMode pm;

        switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            newVal = "Wireframe";
            pm = Ogre::PM_WIREFRAME;
            break;
        case Ogre::PM_WIREFRAME:
            newVal = "Points";
            pm = Ogre::PM_POINTS;
            break;
        default:
            newVal = "Solid";
            pm = Ogre::PM_SOLID;
        }

        mCamera->setPolygonMode(pm);
        mDetailsPanel->setParamValue(10, newVal);
    }
    else if(arg.key == OIS::KC_F5)   // refresh all textures
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }
    return true;
}

bool BaseApplication::keyReleased(const OIS::KeyEvent &evt){}
bool BaseApplication::mouseMoved(const OIS::MouseEvent &evt){}
bool BaseApplication::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){}
bool BaseApplication::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}

void BaseApplication::modifyScore(int num)
{
    score += num;
    if(score<=0)
        score = 0;
    scorePanel->setParamValue(0,Ogre::StringConverter::toString(score));
}

OgreBites::SdkTrayManager* BaseApplication::getTrayManager(void)
{
    return mTrayMgr;
}

Ogre::RenderWindow* BaseApplication::getRenderWindow(void)
{
    return mWindow;
}

OgreBites::ParamsPanel* BaseApplication::getPanel(Panel p)
{
    switch (p) {
        case SCORE: return scorePanel;
        //case INSTRUCT: return instructPanel;
        // case OBJECTIVE: return objectivePanel;
        case POWER: return powerUpPanel;
        case GAMEOVER: return gameOverPanel;
        case GAMEOVERWIN: return gameOverWinPanel;
        case GAMEOVERLOSS: return gameOverLossPanel;
        default: assert(false);
    }
}
OgreBites::Label* BaseApplication::getLabel(Label l)
{
    switch (l) {
        case PAUSE: return pauseLabel;
        default: assert(false);
    }
}
