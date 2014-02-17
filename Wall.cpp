#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition)
	: GameObject(nym, mgr, sim)
{
	// 1x1x1 initially set by Ogre
	Ogre::Entity* ent = mgr->createEntity("FloorEntity", "cube.mesh");

	rootNode->attachObject(ent);
	rootNode->scale(dimensions.x/100, dimensions.y/100, dimensions.z/100);
	ent->setMaterialName("Examples/Rockwall");
}