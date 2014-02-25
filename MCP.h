#ifndef __MCP_h_
#define __MCP_h_

#include <stdlib.h>
#include "BaseApplication.h"

#include "Simulator.h"
#include "OgreMotionState.h"
#include "PlayerCamera.h"
#include "GameObject.h"
#include "Player.h"
#include "Disk.h"
#include "Room.h"
#include "Wall.h"
#include "Target.h"

//#include "ControlListener.h"
class Simulator;

class MCP : public BaseApplication
{
public:
	Simulator* game_simulator;
	Room *room;
	bool gameStart;
	Ogre::Real minutes;
	Ogre::Real seconds;
	bool vKeyDown;

	MCP(void);
	virtual ~MCP(void);

protected:
	virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
};

#endif // #ifndef __MCP_h_
