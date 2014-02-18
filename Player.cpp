#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, btScalar height, btScalar radius, Ogre::Vector3 dimensions, Ogre::Vector3 iposition) 
	: GameObject(nym, mgr, sim)
{
	typeName = "Player";
	isHolding = false;
	// Need position, to translate.
	// Dimensions - so you can make the 
	Ogre::Entity* ent = mgr->createEntity("PlayerEntity", "robot.mesh");

	// need another scenenode for holding the ball
	// need a position of the hand to translate this scenenode to

	rootNode->attachObject(ent);
	rootNode->scale(0.01, 0.01, 0.01);
	rootNode->translate(0,0.1,0);
	ent->setMaterialName("Examples/Robot");

	shape = new btCapsuleShape(radius, height);
	mass = 0.1f;
}

void setPlayerHit(String typeOfObjectCollided)
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

void Player::update(float elapsedTime)
// {
// 	while(sim->checkHit()){
		
// 	}
}