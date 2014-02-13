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
	// initializing light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0,0,0));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		/* more light sources here */

	// setting up the camera
	mCamera->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
    mCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, 0.0f));

	// 10 planes for the room
}
//-------------------------------------------------------------------------------------
bool MCP::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    bool ret = BaseApplication::frameRenderingQueued(evt);

    if (!ret)
        return ret;

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
