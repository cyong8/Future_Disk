#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	// initialize Cameras
	this->pSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(0.0f, 0.0f, -8.0f));
	this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(0.0f, 2.0f, 20.0f));

	this->dimensions = dimensions;
	typeName = "Player";

	isHolding = false; // Is the player holding the disk?
	isInHand = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh"); // Create entity;apply mesh
	rootNode->attachObject(ent); 	// Attach player to a scene node
	rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(position); // Set the position of the player
	ent->setMaterialName("Examples/Robot");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 
	mass = 0.5f; // Set mass of player
}

void Player::attachDisk(Disk* d)
{
	this->isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	d->getSceneNode()->setInheritScale(false);	// Set Inherit Scale to false so that the disk is not scaled down WRT the Player
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player

	d->getBody()->setLinearVelocity(btVector3(0.0f , 0.0f, 0.0f));// make the disk stop moving
	d->updateTransform(); 	// move btRigidBody WRT to the scenenode
	d->getBody()->setActivationState(DISABLE_SIMULATION); // set the activation state of the body so it doesn't move in bullet

 	// DEBUGGING
 	// /*
 	playerDisk->getSceneNode()->showBoundingBox(true);
	this->rootNode->showBoundingBox(true);
 	this->rootNode->setVisible(false, false);
 	// */
}

/*
	return the disk that the player is holding on to
*/
Disk* Player::getPlayerDisk()
{
	return (*this).playerDisk;
}

/*
	
*/
void Player::setHolding()
{
	isHolding = !isHolding;
}

bool Player::checkHolding()
{
	return this->isHolding;
}

/*
	throw the disk
	To do this you need to call setHolding() to release the disk, then send the disk in the direction of the sightNode of the player. 
*/
void Player::throwDisk()
{
	// Get position vector of sight node
	Ogre::Vector3 posVector = pSightNode->getPosition();
	// Set linear velocity vector to 2x along sight node vector
	playerDisk->getBody()->setLinearVelocity(btVector3(posVector.x, posVector.y, posVector.z));

	playerDisk->getBody()->setActivationState(DISABLE_DEACTIVATION); // set the activation state of the body so it doesn't move in bullet
	playerDisk->updateTransform(); 	// move btRigidBody WRT to the scenenode	
}
Ogre::SceneNode* Player::getPlayerSightNode()
{
	return this->pSightNode;
}
Ogre::SceneNode* Player::getPlayerCameraNode()
{
	return this->pCamNode;
}
Ogre::Vector3 Player::getPlayerDimensions()
{
	return this->dimensions;
}