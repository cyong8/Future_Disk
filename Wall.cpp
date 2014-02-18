#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";
	// 1x1x1 initially set by Ogre
	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh");
	
	rootNode->attachObject(ent);
	rootNode->scale(dimensions.x/50.0f, dimensions.y/50.0f, dimensions.z/50.0f);
	rootNode->setPosition(position);
	ent->setMaterialName("Examples/Rockwall");

	//shape = 
}
