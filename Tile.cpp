#include "Tile.h"

Tile::Tile(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 position, Ogre::Real tileSize)
	: GameObject(nym, mgr, sim)
{
	/*
	typeName = "Tile";
	pos = position;
 	//Ogre::Vector3 *floor_dimensions = new Ogre::Vector3(tileSize, 0.01f, tileSize);

	//create entity for plane
	//Ogre::Entity* ent = mgr->createEntity(nym+"Entity", "cube.mesh");
	rootNode->attachObject(ent);
	//rootNode->scale(tileSize/100, 0.01/100, tileSize/100);
	ent->setMaterialName("2 - Default");
	ent->setCastShadows(false);
	rootNode->setPosition(position);

	mass = 0.0f;

	shape = new btBoxShape(btVector3(tileSize/2.0f, 0.01f/2.0f, tileSize/2.0f));
	*/
}