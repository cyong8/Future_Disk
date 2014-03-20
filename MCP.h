#ifndef __MCP_h_
#define __MCP_h_

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

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "Music.h"
#include "Network.h"


class Simulator;
class Music;

class MCP : public BaseApplication
{
public:
	Simulator* gameSimulator;
	Room* gameRoom;
	vector<Wall*> wallList;
	Ogre::Real minutes;
	Ogre::Real seconds;
	bool vKeyDown;
	bool gameStart;
	bool gamePause;
	bool gameOver;
	bool jumpFlag;
	bool allowJumping;

	// MUSIC OBJECT
	Music* gameMusic;
	
	CEGUI::OgreRenderer* mRenderer;

	MCP(void);
	virtual ~MCP(void);

protected:
	virtual void createScene(void);
	void createTargetModeScene(void);
	void createMultiplayerModeScene(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent &evt);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	bool startGame(const CEGUI::EventArgs &e);
	bool hostGame(const CEGUI::EventArgs &e);
	bool joinGame(const CEGUI::EventArgs &e);
	void togglePause(void);
	void gameOverScreen(void);
	bool createMultiplayerMenu(const CEGUI::EventArgs &e);
	void createGUI(void);
	bool quit(const CEGUI::EventArgs &e);
	void createOverlays(PlayerCamera* playCam);
	void restrictPlayerMovement(Player* p);

	Ogre::Light* pointLight;
	Disk* gameDisk;
	Player* hostPlayer;
	Player* clientPlayer;
};

#endif // #ifndef __MCP_h_
