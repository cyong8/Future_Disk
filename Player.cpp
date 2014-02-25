#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	crossHairDimensions = Ogre::Vector3(0.02f, 0.1f, 0.1f);
	// initialize Cameras
	this->pSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(0.0f, 0.0f, -8.0f));
	this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(0.0f, 2.0f, 20.0f));
	this->crossHairNode = pSightNode->createChildSceneNode("CrossHairNode");
	// Entities for Crosshair
	Ogre::Entity* crossHair1 = mgr->createEntity("CrossHair1", "cube.mesh");
	Ogre::Entity* crossHair2 = mgr->createEntity("CrossHair2", "cube.mesh");
	// Attach the Entities
	pSightNode->attachObject(crossHair1);
	crossHairNode->attachObject(crossHair2);
	// Scale the Nodes
	pSightNode->scale(crossHairDimensions.x/100, crossHairDimensions.y/100, crossHairDimensions.z/100);
	crossHairNode->scale(crossHairDimensions.x/100, crossHairDimensions.y/100, crossHairDimensions.z/100);
	// Rotate Nodes so they look like an X
//	pSightNode->rotate();
//	crossHairNode->rotate();
	// Initialize Nodes to invisible and toggle in Aim View
//	pSightNode->setVisible(false);
//	crossHairNode->setVisible(false);

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
void Player::throwDisk()
{
	// Get position vector of sight node
	Ogre::Vector3 posVector = pSightNode->getPosition();
	// Set linear velocity vector to 2x along sight node vector
	playerDisk->getBody()->setLinearVelocity(btVector3(posVector.x, posVector.y, posVector.z));

	playerDisk->getBody()->setActivationState(DISABLE_DEACTIVATION); // set the activation state of the body so it doesn't move in bullet
	playerDisk->updateTransform(); 	// move btRigidBody WRT to the scenenode
	
}

bool Player::checkIsInHand()
{
	return this->isInHand;
}
void Player::setInHand() // is this being called at all (refer to StepSimulation)
{
	this->getPlayerDisk()->getSceneNode()->setPosition(this->rootNode->getPosition() + Ogre::Vector3(this->dimensions.x, 0.0f, 0.0f));
	this->isInHand = true;
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