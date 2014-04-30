#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, int playID, Ogre::Vector3 roomDims) 
	: GameObject(nym, mgr, sim)
{
	initializeStates();
	this->dimensions = dimensions;
	roomDimensions = roomDims;		// Ogre::Vector3(room->width, room->height, numberOfPlayers)
	typeName = "Player";
	groundY = -99999.0f;
	prevGroundY = -99999.0f;
	jumpFactor = 8.0f;
	jumpTimer = 0;
	jumpPowerActive = false;
	movementRestricted = false;
	isHolding = false; // Is the player holding the disk?
	groundConstantSet = false;
	playerID = playID;
	playerCanCatch = true;
	animationState = NULL;

	Ogre::Vector3 position;

	if (playerID == 1)
	{
		playerSide = "Positive Side";
		if (roomDimensions.z > 2)
			position = Ogre::Vector3(-(roomDimensions.x/roomDimensions.y), 0.0f, (roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
		else 
			position = Ogre::Vector3(0.0f, 0.0f, (roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
		rootNode->yaw(Ogre::Radian(Ogre::Math::PI));
	}
	else if (playerID == 2)
	{
		playerSide = "Negative Side";
		if (roomDimensions.z > 2)
			position = Ogre::Vector3(roomDimensions.x/roomDimensions.y, 0.0f, (roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
		else 
			position = Ogre::Vector3(0.0f, 0.0f, -(roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
	}
	else if (playerID == 3)
	{
		playerSide = "Left Side";
		position = Ogre::Vector3(-(roomDimensions.x/roomDimensions.y), 0.0f, -(roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
		rootNode->yaw(Ogre::Radian(Ogre::Math::PI));
	}
	else if (playerID == 4)
	{
		playerSide = "Right Side";
		position = Ogre::Vector3(roomDimensions.x/roomDimensions.y, 0.0f, -(roomDimensions.x/3.0f + roomDimensions.y)/roomDimensions.z);
	}
	startingPosition = position;

	tailParticle = mgr->createParticleSystem("CyanSun_" + nym, "Examples/CyanSun");
    particleNode = rootNode->createChildSceneNode("PlayerParticle_" + nym);
    
    particleNode->attachObject(tailParticle);
    particleNode->setVisible(false);

	ent = mgr->createEntity(nym, "Waddlelly.mesh"); // Create entity;apply mesh
	rootNode->attachObject(ent); 	// Attach player to a scene node
	rootNode->scale(dimensions.x/25.0, dimensions.y/25.0, dimensions.z/25.0);
	// rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(position); // Set the position of the player
	ent->setMaterialName("w_texture_1Material");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 
	mass = 0.5f; // Set mass of player

	// initialize Cameras
	if (playerID == 1)
	{
		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 0.0f, -15.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 4.0f, 15.00));
	}
	else if (playerID == 2)
	{	
		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 0.0f, 15.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 4.0f, -15.00));
	}
	else if (playerID == 3)
	{
		
	}
	else if (playerID == 4)		
	{

	}

 	// DEBUGGING 
	// Ogre::Entity* camEnt = mgr->createEntity(nym+"_camMesh", "cube.mesh");
	// pCamNode->attachObject(camEnt);
	// pCamNode->scale(dimensions.x/5.0, dimensions.y/5.0, dimensions.z/5.0);
	// pCamNode->setVisible(true, false);
	// pCamNode->showBoundingBox(true);
	// camEnt->setMaterialName("Examples/Chrome");

	// Ogre::Entity* sightEnt = mgr->createEntity(nym+"_sightMesh", "cube.mesh");
	// pSightNode->attachObject(sightEnt);
	// pSightNode->scale(dimensions.x/5.0, dimensions.y/5.0, dimensions.z/5.0);
	// pSightNode->setVisible(true, false);
	// pSightNode->showBoundingBox(true);
	// sightEnt->setMaterialName("Examples/Chrome");
	
	// rootNode->setVisible(false, false);
	// this->rootNode->showBoundingBox(true);
}
//-------------------------------------------------------------------------------------
void Player::setHolding(bool x)
{
	isHolding = x;
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
void Player::attachDisk(Disk* d)
{
	isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	d->getSceneNode()->setInheritScale(false);	// Set Inherit Scale to false so that the disk is not scaled down WRT the Player
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player
}
//-------------------------------------------------------------------------------------
Ogre::String Player::getPlayerSide()
{
	return playerSide;
}
//-------------------------------------------------------------------------------------
void Player::setMovementRestriction(bool x)
{
	movementRestricted = x;
}
//-------------------------------------------------------------------------------------
bool Player::checkMovementRestriction()
{
	return movementRestricted;
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
int Player::getPlayerID(void)
{
	return playerID;
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
	// if (groundConstantSet == false)
	// 	groundY = rootNode->getPosition().y;
	// if (/*!(rootNode->getPosition().y > groundY) && */states[JUMP] == false)
	if (states[JUMP] == false)
	{	
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0.0f, jumpFactor, 0.0f));
	    groundConstantSet = true;
	    states[JUMP] = true;
	    /* PLAY MUSIC HERE 	*/
	    return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------
Ogre::String Player::checkPlayerSide()
{
}
//-------------------------------------------------------------------------------------
void Player::increaseJump() 
{
    jumpFactor = 12.0f;
    jumpTimer = 3000;
    jumpPowerActive = true;
    particleNode->setVisible(true);
}
//-------------------------------------------------------------------------------------
void Player::decreaseJump() 
{
    jumpFactor = 8.0f;
    jumpPowerActive = false;
    particleNode->setVisible(false);
}
//-------------------------------------------------------------------------------------
void Player::initializeStates()
{
	int i = VIEWMODE;
	while (i >= 0)
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
void Player::setState(int index, bool value)
{
	states[index] = value;
}
//-------------------------------------------------------------------------------------
Ogre::Vector3 Player::fillVelocityVector(Ogre::Real m, float sprintFactor)
{
    Ogre::Vector3 velocityVector = Ogre::Vector3(0.0f, 0.0f, 0.0f);

	if (states[LEFT])
		velocityVector += Ogre::Vector3(m, 0.0f, 0.0f);
	if (states[RIGHT])
		velocityVector += Ogre::Vector3(-m, 0.0f, 0.0f);
	if (states[BACK])
		velocityVector += Ogre::Vector3(0.0f, 0.0f, -m);
	if (states[FORWARD])
		velocityVector += Ogre::Vector3(0.0f, 0.0f, m);
	if (states[BOOST])
		velocityVector *= sprintFactor;

	return velocityVector;
}
//-------------------------------------------------------------------------------------
bool Player::checkPlayerCanCatch()
{
	return playerCanCatch;
}
//-------------------------------------------------------------------------------------
bool Player::togglePlayerCanCatch()
{
	playerCanCatch = !playerCanCatch;
}
