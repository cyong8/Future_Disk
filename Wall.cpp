#include "Wall.h"

Wall::Wall(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 plane, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Wall";
	Ogre::Entity* ent = mgr->createEntity(nym, "cube.mesh"); // 1x1x1 initially set by Ogre
	
	rootNode->attachObject(ent); // Attach wall to scenenode
	rootNode->scale(dimensions.x/100, dimensions.y/100, dimensions.z/100); // Scale the wall to fit the world
	rootNode->setPosition(position); // Set the position of the wall
	//ent->setMaterialName("Examples/WaterStream");
	ent->setMaterialName("2 - Default");

	shape = new btBoxShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2));
	mass = 0.0f;

	if (nym == "frontwall")
	{
		rootNode->setVisible(false);
	}
}
