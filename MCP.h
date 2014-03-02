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

#include "Music.h"


class Simulator;
class Music;

class MCP : public BaseApplication
{
public:
	Simulator* game_simulator;
	vector<Wall> wallList;
	Ogre::Real minutes;
	Ogre::Real seconds;
	int p1score; // Score for the first player
	bool vKeyDown;
	bool gameStart;
	bool gamePause;
	bool gameOver;
	bool jumpFlag;

	// MUSIC OBJECT
	Music* gameMusic;

	MCP(void);
	virtual ~MCP(void);

protected:
	virtual void createScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool keyPressed(const OIS::KeyEvent &evt);
	Ogre::Light* pointLight;
	Disk* gameDisk;
};

#endif // #ifndef __MCP_h_
