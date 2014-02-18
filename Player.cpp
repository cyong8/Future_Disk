#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, btScalar height, btScalar radius, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	// bool: returns true if player is holding a disk
	isHolding = false;
	// double: gives the scale of the player
	double scale = 0.1; 

	// Need position, to translate.
	// Dimensions - so you can make the 
	// Creates a Ogre::Entity for the player using a mesh of our choice
	Ogre::Entity* ent = mgr->createEntity(nym, "robot.mesh");

	// Attach player to a scene node
	rootNode->attachObject(ent);
	// Scale the player to fit the world
	rootNode->scale(dimensions.x/worldScale, dimensions.y/worldScale, dimensions.z/worldScale);
	// Set the position of the player
	rootNode->setPosition(position);
	// Translate the player - possibly not necesssary? (Kaitlin)
	rootNode->translate(0,0.1,0);

	ent->setMaterialName("Examples/Robot");

	// Set collision shape for Bullet
	shape = new btCapsuleShape(radius, height);
	// Set mass of player
	mass = 0.1f;
}

void Player::setHolding()
{
	isHolding = !isHolding;
}

//void Player::update(float elapsedTime)
// {
// 	while(sim->checkHit()){
		
// 	}
//}
