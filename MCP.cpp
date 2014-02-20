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
    // initialize random number generate
    srand(time(0));

    game_simulator = new Simulator();
    gameStart = false;
    /******************** LIGHTS ********************/
	// initializing light
    mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5,0.5,0.5));
    mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    		/* more light sources here */
    Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setPosition(Ogre::Vector3(0.1f,0.0f, 0.0f));
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);

    /******************** CAMERAS ********************/

	// change the initially positioned camera from the BaseApplication (see BaseApplication::createCamera)

    /******************** ENTITIES ********************/
    //Ogre::Vector3 dimensions = Ogre::Vector3(1.0f, 1.0f, 1.0f);
    //Ogre::Vector3 position = Ogre::Vector3(1.0f, 1.0f, 1.0f);

    // Initialize the disk
  //  (new Disk("Disk", mSceneMgr, game_simulator, Ogre::Math::RangeRandom(0,1)))->addToSimulator();
    // Initialize player1
    (new Player("Player1", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 1.0f, 1.0f), Ogre::Vector3(1.0f, 1.0f, 1.0f)))->addToSimulator();
    // Initialize the room
    new Room(mSceneMgr, game_simulator);
}
//-------------------------------------------------------------------------------------
bool MCP::processUnbufferedInput(const Ogre::FrameEvent& evt)
{
    static bool mMouseDown = false;     // If a mouse button is depressed
    static Ogre::Real mToggle = 0.0;    // The time left until next toggle
    static Ogre::Real mRotate = 0.13;   // The rotate constant
    static Ogre::Real mMove = 3.0f;      // The movement constant
    float fx = 0.0f;
    float fy = 0.0f;
    float fz = 0.0f;
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left);
    bool keyWasPressed = false;
 
    // Tutorial code to possibly help us with throwing the disk:
    // toggles a light currently when the mouse click is released
/*    if (currMouse && ! mMouseDown)
    {
        Ogre::Light* light = mSceneMgr->getLight("pointLight");
        light->setVisible(! light->isVisible());
    }
    if ((mToggle < 0.0f ) && mKeyboard->isKeyDown(OIS::KC_1))
    {
        mToggle  = 0.5;
        Ogre::Light* light = mSceneMgr->getLight("pointLight");
        light->setVisible(! light->isVisible());
    }
 
 */
    mMouseDown = currMouse;
 
    mToggle -= evt.timeSinceLastFrame;
 
    /* Use for rotation of Player by mouse */
 //   this->tdynamicnode->pitch(Ogre::Degree(tpitchchange),Ogre::Node::TS_LOCAL);               
 //   this->tdynamicnode->yaw(Ogre::Degree(tyawchange),Ogre::Node::TS_WORLD); 
 
    btVector3 velocityVector = btVector3(0.0f, 0.0f, 0.0f);
 
    if (mKeyboard->isKeyDown(OIS::KC_I)) // Forward
    {
        fz -= mMove;
        velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
        keyWasPressed = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_K)) // Backward
    {
        fz += mMove;
        velocityVector = velocityVector + btVector3(0.0f, 0.0f, fz);
        keyWasPressed = true;
    }
    if (mKeyboard->isKeyDown(OIS::KC_J)) // Left - yaw or strafe
    {
        fx -= mMove; // Strafe left
        velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
        keyWasPressed = true;
        
    }
    if (mKeyboard->isKeyDown(OIS::KC_L)) // Right - yaw or strafe
    {
        fx += mMove; // Strafe right
        velocityVector = velocityVector + btVector3(fx, 0.0f, 0.0f);
        keyWasPressed = true;
    }
    if (keyWasPressed == true)
    {
        GameObject* p1 = game_simulator->getGameObject((Ogre::String)"Player1");
        p1->getBody()->setLinearVelocity(velocityVector);
    }
    return true;
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
    // handle collisions

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
