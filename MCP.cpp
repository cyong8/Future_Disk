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
    game_simulator = new Simulator();

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
    Ogre::Vector3 *dimensions = new Ogre::Vector3(1.0f, 1.0f, 1.0f);
    Ogre::Vector3 *position = new Ogre::Vector3(1.0f, 1.0f, 1.0f);
    float worldScale = 100.0f;

    // Initialize player1
    new Player("Player1", mSceneMgr, game_simulator, 0.1, 0.1, *dimensions, *position);
    // Initialize the room
    new Room(mSceneMgr, game_simulator);
    // Initialize the disk
    new Disk("Disk", mSceneMgr, game_simulator, 0.1, 0.1, Ogre::Vector3(2.0f, 2.0f, 2.0f));
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);
    
    if (!ret)
        return ret;

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
