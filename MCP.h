#ifndef __MCP_h_
#define __MCP_h_

#include <stdlib.h>
#include "Simulate.h"
#include "OgreMotionState.h"
#include "GameObject.h"

#include "BaseApplication.h"

class MCP : public BaseApplication, 
			public Simulator, 
			public GameObject,
			public OgreMotionState
{


protected:
	virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};


#endif // #ifndef __MCP_h_