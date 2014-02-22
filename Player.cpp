#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
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
	d->getSceneNode()->setPosition(this->rootNode->getPosition()); // set position equal to parent's 
	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player
	// move SceneNode 
	d->getSceneNode()->translate(Ogre::Vector3(dimensions.x/100.0f, 0.0f, 0.0f));
	// make the disk stop moving
	d->getBody()->setLinearVelocity(btVector3(0.0f , 0.0f, 0.0f));
	// move btRigidBody WRT to the SceneNode
	d->updateTransform();
	// set the activation state of the body so it doesn't move in bullet
	d->getBody()->setActivationState(DISABLE_SIMULATION);

	player_disk = d; // player now has a pointer to this disk
	this->isHolding = true;
}

Disk* Player::getPlayerDisk()
{
	return (*this).player_disk;
}

void Player::setHolding()
{
	isHolding = !isHolding;
}

bool Player::checkHolding()
{
	return this->isHolding;
}