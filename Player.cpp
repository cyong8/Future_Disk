#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) 
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

	rootNode->scale(dimensions.x/32.0, dimensions.y/50.0, dimensions.z/49.0);

	// Set the position of the player
	rootNode->setPosition(position);

	//rootNode->translate(0,0.1,0);
	ent->setMaterialName("Examples/Robot");

	// Set collision shape for Bullet
	//shape = new btCapsuleShape(dimensions.x/2, dimensions.y/10);
	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2));

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
