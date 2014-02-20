#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 plane, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";
	// 1x1x1 initially set by Ogre
	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh");
	
	// Attach wall to scenenode
	rootNode->attachObject(ent);
	// Scale the wall to fit the world
	rootNode->scale(dimensions.x/100, dimensions.y/100, dimensions.z/100);
	// Set the position of the wall
	rootNode->setPosition(position);
	ent->setMaterialName("Examples/Rockwall");

	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2));
	mass = 0.0f;
}
