#include "Player.h"

Player::Player(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim)
{
	name = nym;
	sceneMgr = mgr;
	simulator = sim;
	rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	shape = btCapsuleShape;
	motionState = NULL;	// set in the addToSimulator function
	tr.setIdentity();
	mass = 0.0f;
	kinematic = false;
	needsUpdates = false;
	inertia.setZero();
	isHolding = false;
}

Player::setHolding()
{
	(*this).isHolding = !isHolding;
}