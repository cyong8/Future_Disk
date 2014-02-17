#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim)
{
	name = nym;
	sceneMgr = mgr;
	simulator = sim;
	rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	shape = btBoxShape;
	motionState = NULL;	// set in the addToSimulator function
	tr.setIdentity();
	mass = 0.0f;
	kinematic = false;
	needsUpdates = false;
	inertia.setZero();
}