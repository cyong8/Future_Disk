#include "MCP.h"

//-------------------------------------------------------------------------------------
MCP::MCP(void)
{
}

//-------------------------------------------------------------------------------------
MCP::~MCP(void)
{
	delete mRoot;
}

//-------------------------------------------------------------------------------------
void MCP::createScene(void)
{
    game_simulator = new Simulator(mSceneMgr);
    vKeyDown = false;

    // initialize random number generate
    srand(time(0));

    gameStart = false;
    /******************** LIGHTS ********************/
	// Ambient light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5,0.5,0.5));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Point light
    pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    
    /******************** GAME OBJECTS ********************/

    PlayerCamera* p1Cam = new PlayerCamera("P1_cam", mSceneMgr, mCamera);
    
    // Add PlayerCamera to Simulator so it can update it when Player moves
    game_simulator->setCamera(p1Cam);

    // Initialize the Room & add Walls to simulator
    gameRoom = new Room(mSceneMgr, game_simulator);
    // Set point light position to be at the roof
    pointLight->setPosition(Ogre::Vector3(0.0f, game_simulator->getGameObject("Ceiling")->getSceneNode()->getPosition().y, 0.0f));
    // Add Disk to simulator
    (new Disk("Disk", mSceneMgr, game_simulator, Ogre::Math::RangeRandom(0,1)))->addToSimulator();
    gameDisk = (Disk*)game_simulator->getGameObject("Disk");
    // Add Player1 to simulator
    (new Player("Player1", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 2.0f, 1.0f), Ogre::Vector3(1.0f, 1.0f, 1.0f)))->addToSimulator();
    // Add Target to simulator every newTarget number of seconds or if there are no targets in targetList
    (new Target("Target", mSceneMgr, game_simulator, Ogre::Vector3(2.0f, 0.01f, 2.0f), Ogre::Vector3(1.0f, .0f, -19.0f)))->addToSimulator();

    //___Crosshair creation___!
    Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();
         // Create an overlay
    Ogre::Overlay* overlay = overlayManager.create( "OverlayName" );

    // Create a panel
    Ogre::OverlayContainer* CHV = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "PanelName" ) );
    CHV->setPosition( 0.5, 0.4 );
    CHV->setDimensions( 0.001, 0.2 );
    CHV->setMaterialName( "BaseWhite" );
    CHV->getMaterial()->setReceiveShadows(false);

    // Add the CHV to the overlay
    overlay->add2D( CHV );

    Ogre::Overlay* overlay2 = overlayManager.create( "OverlayName2" );
    // // Create a panel
    Ogre::OverlayContainer* CHH = static_cast<Ogre::OverlayContainer*>( overlayManager.createOverlayElement( "Panel", "PanelName2" ) );
    CHH->setPosition( 0.425, 0.5 );
    CHH->setDimensions( 0.15, 0.001 );
    CHH->setMaterialName( "BaseWhite" );
    CHH->getMaterial()->setReceiveShadows(false);
    // Add the CHH to the overlay
    overlay2->add2D( CHH );


    // Show the overlay
    overlay->hide();
    overlay2->hide();

    p1Cam->setCHOverlays(overlay, overlay2);

}

//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    static bool mMouseDown = false;     // If a mouse button is depressed
    static Ogre::Real mMove = 3.0f;      // The movement constant
    float fx = 0.0f;
    float fy = 0.0f;
    float fz = 0.0f;
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);
    bool keyWasPressed = false;
    float sprintFactor = 1.0f;
 
    Player *p = (Player *)game_simulator->getGameObject("Player1");
        
    if(!mMouseDown && currMouse && p->checkHolding() && vKeyDown)
    {
        //Ogre::LogManager::getSingletonPtr()->logMessage("\n\n\n\n_______setting throw flag_______\n\n\n\n");
        game_simulator->setThrowFlag();
    }
    mMouseDown = currMouse;
    // Default velocity vector - this can be changed if we want to sprint
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);

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
    // Sprint mode - press spacebar to activate
    if(mKeyboard->isKeyDown(OIS::KC_SPACE))
    {
        sprintFactor = 3.0f;
    }
    else
    {
        sprintFactor = 1.0f;
    }


    if (!vKeyDown)  // disable movement while in aim mode
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
        if (keyWasPressed == true)
        {
            Player* p1 = (Player*)(game_simulator->getGameObject((Ogre::String)"Player1"));
            p1->getBody()->setLinearVelocity(velocityVector * sprintFactor);
        }
    }

    return true;
}

bool MCP::mouseMoved(const OIS::MouseEvent &evt)
{
    // if 'v' is pressed and was pressed last frame - still in aim mode
    if (vKeyDown)
    {
        Player* p = (Player*)game_simulator->getGameObject("Player1");
        p->getPlayerSightNode()->translate(evt.state.X.rel/25.0f, -evt.state.Y.rel/25.0f, 0);
    }
}


//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

    bool ret = BaseApplication::frameRenderingQueued(evt);
 
    if(!processUnbufferedInput(evt)) 
        return false;

    game_simulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f);
    // check collisions
    game_simulator->setHitFlags();

    // Light tracking Disk
    pointLight->setPosition(gameDisk->getSceneNode()->getPosition().x, pointLight->getPosition().y, gameDisk->getSceneNode()->getPosition().z);

    return ret;
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
