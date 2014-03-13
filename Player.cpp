#include "Player.h"
#include "Disk.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	this->dimensions = dimensions;
	typeName = "Player";
	groundY = 0.0f;

	isHolding = false; // Is the player holding the disk?
	groundConstantSet = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh"); // Create entity;apply mesh
	rootNode->attachObject(ent); 	// Attach player to a scene node
	rootNode->scale(dimensions.x/100.0, dimensions.y/100.0, dimensions.z/100.0);
	rootNode->setPosition(position); // Set the position of the player
	ent->setMaterialName("Examples/Chrome");
	// Set collision shape for Bullet
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2)); 
	mass = 0.5f; // Set mass of player

	// initialize Cameras
	this->pSightNode = rootNode->createChildSceneNode(nym + "_sight", Ogre::Vector3(0.0f, 0.0f, -1350.0f));
	this->pCamNode = rootNode->createChildSceneNode(nym + "_camera", Ogre::Vector3(0.0f, 200.0f, 1350.0f));

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


	this->rootNode->showBoundingBox(true);*/
}

void Player::attachDisk(Disk* d)
{
	isHolding = true;
	playerDisk = d; // player now has a pointer to this disk

	d->getSceneNode()->getParent()->removeChild(d->getSceneNode()); // detach the disk from it's parent (root or other player)
	d->getSceneNode()->setInheritScale(false);	// Set Inherit Scale to false so that the disk is not scaled down WRT the Player
	this->getSceneNode()->addChild((d->getSceneNode())); // Set disk's parent to this player

 	// DEBUGGING
 	// playerDisk->getSceneNode()->showBoundingBox(true);
}
void Player::setHolding()
{
	isHolding = !isHolding;
}
bool Player::checkHolding()
{
	return isHolding;
}
void Player::throwDisk()
{
}
Disk* Player::getPlayerDisk()
{
	return playerDisk;
}
Ogre::SceneNode* Player::getPlayerSightNode()
{
	return pSightNode;
}
Ogre::SceneNode* Player::getPlayerCameraNode()
{
	return pCamNode;
}
Ogre::Vector3 Player::getPlayerDimensions()
{
	return dimensions;
}
void Player::setGroundY(float y)
{
	groundY = y;
	groundConstantSet = true;           	
}
float Player::getGroundY()
{
	return groundY;
}