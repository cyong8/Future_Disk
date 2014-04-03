#include "Server.h"

Server::Server(MCP* mcp)
{
	MasterControl = mcp;
	gameMusic = mcp->getMusicObject();
	sSceneMgr = mcp->getSceneManager();

	// gameNetwork = new Network();
	gameSimulator = new Simulator(sSceneMgr, gameMusic);

}
//-------------------------------------------------------------------------------------
Server::~Server(void)
{
}
//-------------------------------------------------------------------------------------
void Server::createScene()
{
    gameRoom = new Room(sSceneMgr, gameSimulator, 0);

    /********************  OBJECT CREATION  ********************/
/*	Add Players when people connect

    (new Player("Player1", sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, 15.0f), "Positive Side"))->addToSimulator(); // Create Player 1
    (new Player("Player2", sSceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), Ogre::Vector3(0.0f, 0.0f, -15.0f), "Negative Side"))->addToSimulator(); // Create Player 2
*/
    Power = new Target("Power", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Speed = new Target("Speed", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    JumpPower = new Target("Jump", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());
    Restore = new Target("Restore", sSceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), Ogre::Vector3(1.0f, 0.0f, -19.0f), gameRoom->getBounds());

    Power->addToSimulator(); 
    Speed->addToSimulator(); 
    JumpPower->addToSimulator(); 
    Restore->addToSimulator(); 

    sSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = sSceneMgr->createLight("pointLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getWall(Ceiling)->getSceneNode()->getPosition().y, 0.0f));
}
//-------------------------------------------------------------------------------------
bool Server::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (!processBufferedInput(evt))
		exit(2);

	gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f); 
    gameSimulator->parseCollisions(); // check collisions
}
//-------------------------------------------------------------------------------------
bool Server::processBufferedInput(const Ogre::FrameEvent& evt)
{

}
//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------