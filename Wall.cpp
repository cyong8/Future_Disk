#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::String planeNym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 norm, Ogre::Real dimension)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";

	//create entity for plane
	Ogre::Entity* ent = mgr->createEntity(planeNym+"Entity", planeNym);
	rootNode->attachObject(ent);
	ent->setMaterialName("2 - Default");
	ent->setCastShadows(false);

	mass = 0.0f;
	shape = new btStaticPlaneShape(btVector3(norm.x, norm.y, norm.z), -dimension/2);				
}
