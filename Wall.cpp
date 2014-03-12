#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Plane op, Ogre::Vector3 norm)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";

	//create entity for plane
	Ogre::Entity* ent = mgr->createEntity(nym+"Entity", nym); // 1x1x1 initially set by Ogre
	ent->setMaterialName("2 - Default");
	ent->setCastShadows(false);
	rootNode->attachObject(ent);

	shape = new btStaticPlaneShape(btVector3(norm.x, norm.y, norm.z));				
	mass = 0.0f;
}
