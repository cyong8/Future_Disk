#ifndef __MCP_h_
#define __MCP_h_

#include <stdlib.h>
#include "BaseApplication.h"
#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
class Simulator;

class MCP : public BaseApplication
{
public:
	Simulator* game_simulator;

	MCP(void);
	virtual ~MCP(void);

protected:
	virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // #ifndef __MCP_h_