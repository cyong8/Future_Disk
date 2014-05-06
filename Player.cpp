#include "Player.h"
#include "Disk.h"
#include "Room.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, int playID, Room* gameRoom)//Ogre::Vector3 roomDims) 
	: GameObject(nym, mgr, sim)
{
	initializeStates();
	this->dimensions = dimensions;
	playerRoom = gameRoom;
	// Ogre::Vector3(room->width, room->height, numberOfPlayers)
	typeName = "Player";
	jumpFactor = 10.0f;
	jumpTimer = 0;
	jumpPowerActive = false;
	movementRestricted = false;
	isHolding = false; // Is the player holding the disk?
	playerID = playID;
	playerCanCatch = true;
	customAnimationState = NULL;
	moving = false;
	catchAnimation = false;

	setPlayerStartingPosition(false);

	tailParticle = mgr->createParticleSystem("CyanSun_" + nym, "Examples/CyanSun");
    particleNode = rootNode->createChildSceneNode("PlayerParticle_" + nym);
    
    particleNode->attachObject(tailParticle);
    particleNode->setVisible(false);

	customPlayerEnt = mgr->createEntity(nym, "Waddlelly.mesh"); // Create entity;apply mesh
	rootNode->attachObject(customPlayerEnt); 	// Attach player to a scene node
	rootNode->scale(dimensions.x/20.0, dimensions.y/20.0, dimensions.z/20.0);
	// rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(startingPosition); // Set the startingPosition of the player
	// customPlayerEnt->setMaterialName("w_texture_1Material");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 
	mass = 0.5f; // Set mass of player

	// initialize Cameras
	if (playerID == 1)
	{
		customPlayerEnt->setMaterialName("w_green");

		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, -5.0f, -20.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 8.0f, 20.00));
	}
	else if (playerID == 2)
	{	
		customPlayerEnt->setMaterialName("w_blue");

		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 0.0f, 15.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 2.0f, -15.00));
	}
	else if (playerID == 3)
	{
		customPlayerEnt->setMaterialName("w_purple");

		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 0.0f, 15.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 2.0f, -15.00));
	}
	else if (playerID == 4)		
	{
		customPlayerEnt->setMaterialName("w_red");

		this->pSightNode = rootNode->createChildSceneNode(nym + "_sight");
		this->pCamNode = rootNode->createChildSceneNode(nym + "_camera");
		this->pSightNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 0.0f, 15.00));
		this->pCamNode->_setDerivedPosition(rootNode->_getDerivedPosition() + Ogre::Vector3(0.0f, 2.0f, -15.00));
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
void Player::attachDisk(Disk* d)
{
	isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	d->getSceneNode()->setInheritScale(false);	// Set Inherit Scale to false so that the disk is not scaled down WRT the Player
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player
	animateCharacter("catch");
}
//-------------------------------------------------------------------------------------
void Player::setMovementRestriction(bool x)
{
	movementRestricted = x;
}
//-------------------------------------------------------------------------------------
bool Player::performJump()
{
	if (states[JUMP] == false)
	{	
		body->setLinearVelocity(body->getLinearVelocity() + btVector3(0.0f, jumpFactor, 0.0f));
	    states[JUMP] = true;
	    /* PLAY MUSIC HERE 	*/
	    return true;
	}
	return false;
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
	states[PLAYING] = true;
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
bool Player::togglePlayerCanCatch()
{
	playerCanCatch = !playerCanCatch;
}
//-------------------------------------------------------------------------------------
void Player::animateCharacter(Ogre::String stateName)
{
	customAnimationState = customPlayerEnt->getAnimationState(stateName);
	customAnimationState->setEnabled(true);

	if(stateName == "walk")
    	customAnimationState->setLoop(true);
    else
    	customAnimationState->setLoop(false);

    customAnimationState->setTimePosition(0);
}
//-------------------------------------------------------------------------------------
void Player::setPlayerSpace()
{
	playerSpace = playerRoom->getPlayerRoomSpace(playerID);
}
//-------------------------------------------------------------------------------------
void Player::changeGameRoom(Room* newGameRoom)
{
	playerRoom = newGameRoom;
	playerSpace = playerRoom->getPlayerRoomSpace(playerID);
}
//-------------------------------------------------------------------------------------
void Player::setPlayerStartingPosition(bool changeRoomFlag)
{
	Ogre::Real roomWidth = playerRoom->getWidth();
	Ogre::Real roomHeight = playerRoom->getHeight();
	int numberOfPlayers = playerRoom->getNumberOfPlayers();

	if (playerID == 1)
	{
		if (numberOfPlayers > 2)
			startingPosition = Ogre::Vector3(-(roomWidth/numberOfPlayers), 0.0f, (roomWidth/3.0f + roomHeight)/numberOfPlayers);
		else 
			startingPosition = Ogre::Vector3(0.0f, 0.0f, (roomWidth/3.0f + roomHeight)/numberOfPlayers);
		
		if (!changeRoomFlag)
			rootNode->yaw(Ogre::Radian(Ogre::Math::PI));
	}
	else if (playerID == 2)
	{
		if (numberOfPlayers > 2)
			startingPosition = Ogre::Vector3(roomWidth/numberOfPlayers, 0.0f, (roomWidth/3.0f + roomHeight)/numberOfPlayers);
		else 
			startingPosition = Ogre::Vector3(0.0f, 0.0f, -(roomWidth/3.0f + roomHeight)/numberOfPlayers);
	}
	else if (playerID == 3)
	{
		startingPosition = Ogre::Vector3(-(roomWidth/numberOfPlayers), 0.0f, -(roomWidth/3.0f + roomHeight)/numberOfPlayers);
		
		if (!changeRoomFlag)
			rootNode->yaw(Ogre::Radian(Ogre::Math::PI));
	}
	else if (playerID == 4)
	{
		startingPosition = Ogre::Vector3(roomWidth/numberOfPlayers, 0.0f, -(roomWidth/3.0f + roomHeight)/numberOfPlayers);
	}

	rootNode->setPosition(startingPosition);
	
	if (changeRoomFlag)
	{
		Ogre::Vector3 pos = rootNode->getPosition();
		btQuaternion rotationQ;
        rotationQ = btQuaternion(rootNode->getOrientation().getYaw().valueRadians(), 0, 0);
        btTransform transform = btTransform(rotationQ, btVector3(pos.x, pos.y, pos.z));

       	body->setCenterOfMassTransform(transform);
	}
}
//-------------------------------------------------------------------------------------