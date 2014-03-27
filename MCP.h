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
	bool cViewModeToggle;
    bool clientVKeyDown;
	int clientServerIdentifier;
	float timeSinceLastStateUpdate;
	vector<char*> termArgs;

	// NETWORK OBJECT
	Network* gameNetwork;
	// MUSIC OBJECT
	Music* gameMusic;
	
	CEGUI::OgreRenderer* mRenderer;
	//Ogre::ManualObject* trajectory;
	MCP(void);
	virtual ~MCP(void);

protected:
	virtual void createScene(void);
	void createSoloModeScene(void);
	void createMultiplayerModeScene_host(void);
	void createMultiplayerModeScene_client(void);
	bool soloMode(const CEGUI::EventArgs &e);
	bool hostGame(const CEGUI::EventArgs &e);
	bool joinGame(const CEGUI::EventArgs &e);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	bool constructAndSendGameState(void);
	bool updateClient(const Ogre::FrameEvent& evt);
	void updateClientCamera(Ogre::Real elapseTime);
	bool processAndSendClientInput(const Ogre::FrameEvent& evt);
	bool interpretClientPacket(MCP_Packet pack);
	bool interpretServerPacket(MCP_Packet pack);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent &evt);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	void togglePause(void);
	void gameOverScreen(void);
	bool createMultiplayerMenu(const CEGUI::EventArgs &e);
	void createGUI(void);
	bool quit(const CEGUI::EventArgs &e);
	void createOverlays(PlayerCamera* playCam);
	void restrictPlayerMovement(Player* p);
	void showTrajectory(PlayerCamera* playCam);

	Ogre::Light* pointLight;
	Disk* gameDisk;			// 'd'
	Player* hostPlayer;		// 'h'
	Player* clientPlayer;	// 'c'
	PlayerCamera* pCam;
	int gameMode;
	int sceneRendered;
	Ogre::Real mMove; 			// Move Constant
	float sprintFactor;	// Sprint Constant
};

#endif // #ifndef __MCP_h_
