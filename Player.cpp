#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position, Ogre::String side) 
	: GameObject(nym, mgr, sim)
{
	initializeStates();
	this->dimensions = dimensions;
	typeName = "Player";
	groundY = -99999.0f;
	prevGroundY = -99999.0f;
	jumpFactor = 8.0f;
	playerSide = side;

	isHolding = false; // Is the player holding the disk?
	groundConstantSet = false;
	
	//tailParticle[0] = mgr->createParticleSystem("CyanSun", "Examples/CyanSun");
	//tailParticle[1] = mgr->createParticleSystem("MagentaSun", "Examples/MagentaSun");

	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh"); // Create entity;apply mesh
	rootNode->attachObject(ent); 	// Attach player to a scene node
	rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(position); // Set the position of the player
	ent->setMaterialName("Examples/Chrome");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 
	mass = 0.5f; // Set mass of player

	// initialize Cameras
	if (playerSide == "Negative Side")
	{
		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(0.0f, 0.0f, 1350.0f));
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(0.0f, 200.0f, -1350.0f));
	}
	else
	{	
		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(0.0f, 0.0f, -1350.0f));
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(0.0f, 200.0f, 1350.0f));
	}

 	// DEBUGGING 
/*	Ogre::Entity* camEnt = mgr->createEntity(nym+"_camMesh", "cube.mesh");
	pCamNode->attachObject(camEnt);
	pCamNode->scale(dimensions.x/5.0, dimensions.y/5.0, dimensions.z/5.0);
	pCamNode->setVisible(true, false);
	pCamNode->showBoundingBox(true);
	camEnt->setMaterialName("Examples/Chrome");

	Ogre::Entity* sightEnt = mgr->createEntity(nym+"_sightMesh", "cube.mesh");
	pSightNode->attachObject(sightEnt);
	pSightNode->scale(dimensions.x/5.0, dimensions.y/5.0, dimensions.z/5.0);
	pSightNode->setVisible(true, false);
	pSightNode->showBoundingBox(true);
	sightEnt->setMaterialName("Examples/Chrome");
	*/
	// rootNode->setVisible(false, false);
	// this->rootNode->showBoundingBox(true);

}
//-------------------------------------------------------------------------------------
void Player::attachDisk(Disk* d)
{
	isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	d->getSceneNode()->setInheritScale(false);	// Set Inherit Scale to false so that the disk is not scaled down WRT the Player
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player
}
//-------------------------------------------------------------------------------------
void Player::setHolding()
{
	isHolding = !isHolding;
}
//-------------------------------------------------------------------------------------
bool Player::checkHolding()
{
	return isHolding;
}
//-------------------------------------------------------------------------------------
void Player::throwDisk()
{
}
//-------------------------------------------------------------------------------------
Disk* Player::getPlayerDisk()
{
	return playerDisk;
}
//-------------------------------------------------------------------------------------
Ogre::SceneNode* Player::getPlayerSightNode()
{
	return pSightNode;
}
//-------------------------------------------------------------------------------------
Ogre::SceneNode* Player::getPlayerCameraNode()
{
	return pCamNode;
}
//-------------------------------------------------------------------------------------
Ogre::Vector3 Player::getPlayerDimensions()
{
	return dimensions;
}
//-------------------------------------------------------------------------------------
void Player::setGroundY(Ogre::Real y)
{
	groundY = y;
	groundConstantSet = true;           	
}
//-------------------------------------------------------------------------------------
float Player::getGroundY()
{
	return groundY;
}
//-------------------------------------------------------------------------------------
bool Player::performJump()
{
	if (groundConstantSet == false)
		groundY = rootNode->getPosition().y;
	if (!(rootNode->getPosition().y > groundY))
	{	
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0.0f, jumpFactor, 0.0f));
	    groundConstantSet = true;
	    return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------
void Player::applyPowerUp(Ogre::String type)
{
    
}
//-------------------------------------------------------------------------------------
Ogre::String Player::checkPlayerSide()
{

}
//-------------------------------------------------------------------------------------
void Player::increaseJump() 
{
    jumpFactor = 12.0f;
}
//-------------------------------------------------------------------------------------
void Player::decreaseJump() 
{
    jumpFactor = 8.0f;
}
//-------------------------------------------------------------------------------------
void Player::initializeStates()
{
	int i = Forward;
	while (i > 0)
	{
		states.push_back(false);
		i--;
	}
}
//-------------------------------------------------------------------------------------
bool Player::checkState(int index)
{
	return states[index];
}
//-------------------------------------------------------------------------------------
void Player::toggleState(int index)
{
	states[index] = !states[index];
}
void Player::resetPlayerState(const Ogre::FrameEvent& evt, OIS::Keyboard* mKeyboard)
{
	if (!mKeyboard->isKeyDown(OIS::KC_W) && this->checkState(Forward))
        this->toggleState(Forward);
    if (!mKeyboard->isKeyDown(OIS::KC_A) && this->checkState(Left))
        this->toggleState(Left);
    if (!mKeyboard->isKeyDown(OIS::KC_S) && this->checkState(Back))
        this->toggleState(Back);
    if (!mKeyboard->isKeyDown(OIS::KC_D) && this->checkState(Right))
        this->toggleState(Right);
    if (!mKeyboard->isKeyDown(OIS::KC_J) && this->checkState(Jump))
    	this->toggleState(Jump);
    if (!mKeyboard->isKeyDown(OIS::KC_LSHIFT) && this->checkState(Boost))
    	this->toggleState(Boost);
}
Ogre::Vector3 Player::fillClientVelocityVector(Ogre::Real m, Ogre::Real sprintFactor)
{
    Ogre::Vector3 velocityVector = Ogre::Vector3(0.0f, 0.0f, 0.0f);

	if (states[Left])
		velocityVector += Ogre::Vector3(m, 0.0f, 0.0f);
	if (states[Right])
		velocityVector += Ogre::Vector3(-m, 0.0f, 0.0f);
	if (states[Back])
		velocityVector += Ogre::Vector3(0.0f, 0.0f, -m);
	if (states[Forward])
		velocityVector += Ogre::Vector3(0.0f, 0.0f, m);
	if (states[Boost])
		velocityVector *= sprintFactor;

	return velocityVector;
}