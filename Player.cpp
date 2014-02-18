#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, btScalar height, btScalar radius, Ogre::Vector3 dimensions, Ogre::Vector3 iposition) 
	: GameObject(nym, mgr, sim)
{
	isHolding = false;

	// Need position, to translate.
	// Dimensions - so you can make the 
	Ogre::Entity* ent = mgr->createEntity("PlayerEntity", "robot.mesh");

	rootNode->attachObject(ent);
	rootNode->scale(0.01, 0.01, 0.01);
	rootNode->translate(0,0.1,0);
	ent->setMaterialName("Examples/Robot");

	shape = new btCapsuleShape(radius, height);
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
