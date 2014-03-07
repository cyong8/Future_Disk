/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.h
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
#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

#include <vector>
#include "btBulletDynamicsCommon.h"

#include <assert.h>
#include <time.h>

using namespace std;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#    define OGRE_IS_IOS 1
#    include <OISMultiTouch.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#   include <macUtils.h>
#endif

#ifdef OGRE_IS_IOS
class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, OIS::KeyListener, OIS::MultiTouchListener, OgreBites::SdkTrayListener
#else
class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, OgreBites::SdkTrayListener
#endif
{
public:
    BaseApplication(void);
    virtual ~BaseApplication(void);

    virtual void go(void);

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
    virtual bool setup();
    virtual bool configure(void);
    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    virtual void createFrameListener(void);
    virtual void createScene(void) = 0; // Override me!
    virtual void destroyScene(void);
    virtual void createViewports(void);
    virtual void setupResources(void);
    virtual void createResourceListener(void);
    virtual void loadResources(void);
    // OIS::KeyListener
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &evt);
    virtual bool mouseMoved(const OIS::MouseEvent &evt);
    virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
    // Ogre::WindowEventListener
    //Adjust mouse clipping area
    virtual void windowResized(Ogre::RenderWindow* rw);
    //Unattach OIS before window shutdown (very important under Linux)
    virtual void windowClosed(Ogre::RenderWindow* rw);
    

    bool updateTimer(time_t currTime);
    void updatePauseTime(time_t currTime);
    void modifyScore(int num);
    Ogre::Real mRotate;

    Ogre::Root *mRoot;
    Ogre::Camera* mCamera;
    Ogre::SceneManager* mSceneMgr;
    Ogre::RenderWindow* mWindow;
    Ogre::String mResourcesCfg;
    Ogre::String mPluginsCfg;

    // OgreBites
    OgreBites::ParamsPanel* scorePanel;       // scoring panel for TronGame
    OgreBites::ParamsPanel* instructPanel;
    OgreBites::ParamsPanel* objectivePanel;
    OgreBites::Label* startLabel;
    OgreBites::Label* pauseLabel;
    OgreBites::ParamsPanel* gameOverPanel;
    int initMinutes;
    time_t initTime;
    double pTimePassed;
    time_t pauseTime;
    int score;
    bool gameOver;
    
    OgreBites::SdkTrayManager* mTrayMgr;
    OgreBites::ParamsPanel* mDetailsPanel;     // sample details panel
    bool mCursorWasVisible;                    // was cursor visible before dialog appeared
    bool mShutDown;

    //OIS Input devices
    OIS::InputManager* mInputManager;
    OIS::Mouse*    mMouse;
    OIS::Keyboard* mKeyboard;
};

#endif // #ifndef __BaseApplication_h_
