#include "Tile.h"

Tile::Tile(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 position, Ogre::Real tileSize)
	: GameObject(nym, mgr, sim)
{
	typeName = "Tile";
	pos = position;
 	Ogre::Vector3 tile_dimensions = Ogre::Vector3(tileSize, 0.01f, tileSize);

	//create entity for plane
	Ogre::Entity* ent = mgr->createEntity(nym+"Entity", "cube.mesh");
	rootNode->attachObject(ent);
	rootNode->scale(tile_dimensions.x/100, tile_dimensions.y/100, tile_dimensions.z/100);
	ent->setMaterialName("2 - Default");
	ent->setCastShadows(false);
	rootNode->setPosition(position);

	mass = 0.0f;

	shape = new btBoxShape(btVector3(tile_dimensions.x/2.0f, tile_dimensions.y/2.0f, tile_dimensions.z/2.0f));
}

void Tile::markHit()
{
	hit = true;
	this->rootNode->setVisible(false);
}

bool Tile::isHit()
{
	return hit;
}
