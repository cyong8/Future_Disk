#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::String planeNym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 norm, Ogre::Vector3 position, Ogre::Vector3 dimensions)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";
	dims = dimensions;
	pos = position;

	//create entity for plane
	Ogre::Entity* ent = mgr->createEntity(planeNym+"Entity", planeNym);
	rootNode->attachObject(ent);
	ent->setMaterialName("2 - Default");
	ent->setCastShadows(false);
	rootNode->setPosition(position);

	mass = 0.0f;
	shape = new btBoxShape(btVector3(dimensions.x/2.0f, dimensions.y/2.0f, dimensions.z/2.0f));
}
