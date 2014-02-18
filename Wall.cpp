#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";
	// 1x1x1 initially set by Ogre
	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh");
	
	// Attach wall to scenenode
	rootNode->attachObject(ent);
	// Scale the wall to fit the world
	rootNode->scale(worldScale, worldScale, worldScale);
	// Set the position of the wall
	rootNode->setPosition(position);
	ent->setMaterialName("Examples/Rockwall");

	//shape = 
}
