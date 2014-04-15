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

#include "Music.h"
#include "Server.h"
#include "Client.h"
#include "GUI.h"
#include "Solo.h"

class Simulator;
class Music;
class GUI;
class Server;
class Client;
class Solo;

class MCP : public BaseApplication
{
public:
	Ogre::Real minutes;
	Ogre::Real seconds;

	/* Primary player for this instance of MCP */
	Client* mainClient;

	vector<char*> termArgs;
	
	// MUSIC OBJECT
	Music* gameMusic;
	// GUI OBJECT
    GUI* gui;	
    // SERVER OBJECT
    Server* gameServer;

	//Ogre::ManualObject* trajectory;
	MCP(void);
	virtual ~MCP(void);
	
	// Solo class in the event of single player
	Solo* solo;

	// Methods for the GUI class to access
	bool soloMode(const CEGUI::EventArgs &e);
	bool hostGame(const CEGUI::EventArgs &e);
	bool joinGame(const CEGUI::EventArgs &e);
	bool quit(const CEGUI::EventArgs &e);
	Ogre::SceneManager* getSceneManager(void);
	Music* getMusicObject(void);


protected:
	virtual void createScene(void);
	void createSoloModeScene(void);
	void createMultiplayerModeScene_host(void);
	void createMultiplayerModeScene_client(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt);
	void renderLoop_Host(const Ogre::FrameEvent& evt);
	void renderLoop_Client(const Ogre::FrameEvent& evt);
	bool constructAndSendGameState(void);
	bool updateClient(const Ogre::FrameEvent& evt);
	void updateClientCamera(Ogre::Real elapseTime);
	bool processAndSendClientInput(const Ogre::FrameEvent& evt);
	bool resetClientState(const Ogre::FrameEvent& evt, vector<MCP_Packet> &packList);
	bool interpretClientPacket(MCP_Packet pack);
	bool interpretServerPacket(MCP_Packet pack);
	void updateRemovedTiles(void);
	virtual bool mouseMoved(const OIS::MouseEvent &evt);
	virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool keyPressed(const OIS::KeyEvent &evt);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	void togglePause(void);
	bool checkGameLoss(Player* p);
	bool activateMainMenuSolo(const CEGUI::EventArgs &e);
	void createOverlays(PlayerCamera* playCam);
	void restrictPlayerMovement(Player* p);
	void showTrajectory(PlayerCamera* playCam);
	void setShutDown(void);
};

#endif // #ifndef __MCP_h_
