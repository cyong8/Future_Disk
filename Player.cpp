#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim) 
	: GameObject(nym, mgr, sim)
{
	isHolding = false;

	// Need position
	// Dimensions - so you can make the 
}

void Player::setHolding()
{
	isHolding = !isHolding;
}