#include "Disk.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim)
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
	heldBy = NULL;
}

void Disk::setPlayer(Player p)
{
	(*this).heldBy = p;
}