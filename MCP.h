#ifndef __MCP_h_
#define __MCP_h_

#include <stdlib.h>
#include "BaseApplication.h"
#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "ControlListener.h"
class Simulator;

class MCP : public BaseApplication
{
public:
	Simulator* game_simulator;

	MCP(void);
	virtual ~MCP(void);

protected:
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif // #ifndef __MCP_h_