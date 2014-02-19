#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, btScalar height, btScalar radius, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	typeName = "Player";

	// bool: returns true if player is holding a disk
	isHolding = false;

	// Need position, to translate.
	// Dimensions - so you can make the 
	// Creates a Ogre::Entity for the player using a mesh of our choice
	Ogre::Entity* ent = mgr->createEntity(nym, "robot.mesh");

	// need another scenenode for holding the ball
	// need a position of the hand to translate this scenenode to

	// Attach player to a scene node
	rootNode->attachObject(ent);
	// Scale the player to fit the world

	rootNode->scale(dimensions.x/32.1252, dimensions.y/101.673, dimensions.z/49.0925);

	// Set the position of the player
	rootNode->setPosition(position);

	//rootNode->translate(0,0.1,0);
	ent->setMaterialName("Examples/Robot");

	// Set collision shape for Bullet
	//shape = new btCapsuleShape(dimensions.x/2, dimensions.y/10);
	shape = new btCapsuleShape(dimensions.x/32.1252, dimensions.y/101.673);

	// Set mass of player
	mass = 0.1f;
}

void Player::setPlayerHit(string typeOfObjectCollided)
{
	if (typeOfObjectCollided == "Wall")
		hit_wall = true;
	if (typeOfObjectCollided == "Disk")
		hit_disk = true;
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
