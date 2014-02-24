#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	// initialize Cameras
	this->mSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(position.x, position.y, -8.0f));
	this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(position.x, position.y, 12.0f));

	this->dimensions = dimensions;
	typeName = "Player";

	isHolding = false; // Is the player holding the disk?

	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh"); // Create entity;apply mesh

	// need another scenenode for holding the ball
	// need a position of the hand to translate this scenenode to

	rootNode->attachObject(ent); 	// Attach player to a scene node

	rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(position); // Set the position of the player
	ent->setMaterialName("Examples/Robot");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 

	// Set mass of player
	mass = 0.1f;
}

void Player::attachDisk(Disk* d)
{
	this->isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	// d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	// d->getSceneNode()->setInheritScale(false);
	// this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player
	
	// Check/Try _setDerivedPosition() instead of setPosition
	d->getSceneNode()->setPosition(this->rootNode->getPosition()); // set position equal to parent's 
	// d->getSceneNode()->translate(Ogre::Vector3(this->dimensions.x, 0, 0));
	d->getSceneNode()->needUpdate(true);

	// make the disk stop moving
	d->getBody()->setLinearVelocity(btVector3(0.0f , 0.0f, 0.0f));
	// move btRigidBody WRT to the scenenode
	d->updateTransform();
	// set the activation state of the body so it doesn't move in bullet
	d->getBody()->setActivationState(DISABLE_SIMULATION);

	// DEBUGGING
	playerDisk->getSceneNode()->showBoundingBox(true);
	this->rootNode->showBoundingBox(true);
	this->rootNode->setVisible(false, false);
}

Disk* Player::getPlayerDisk()
{
	return (*this).playerDisk;
}

void Player::setHolding()
{
	isHolding = !isHolding;
}

bool Player::checkHolding()
{
	return this->isHolding;
}

Ogre::SceneNode* Player::getPlayerSightNode()
{
	return this->mSightNode;
}

Ogre::SceneNode* Player::getPlayerCameraNode()
{
	return this->pCamNode;
}

Ogre::Vector3 Player::getPlayerDimensions()
{
	return this->dimensions;
}