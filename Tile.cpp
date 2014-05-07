#include "Tile.h"

Tile::Tile(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 position, Ogre::Real tileSize, int tileNum, int pID)
	: GameObject(nym, mgr, sim)
{
	typeName = "Tile";
	pos = position;
	tileNumber = tileNum;
	tileOwner = pID;
 	Ogre::Vector3 tile_dimensions = Ogre::Vector3(tileSize, 0.01f, tileSize);

 	/* 
		Explosion Particle System from Ogre Website:
			http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Explosion&structure=Cookbook
	*/
	/*ParticleSystem* particleSystem = sceneManager->createParticleSystem("explosions", "explosionTemplate");
	particleSystem->fastForward(1.0); 	// fast forward 1 second  to the point where the particle has been emitted
	sceneNode->attachObject(particleSystem); // attach the particle system to a scene node
	*/

 	// explosionMaterial - particle explosion 
 	// explosionParticle = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");
 	// explosionNode = rootNode->createChildSceneNode(nym + Ogre::StringConverter::toString("_particle_node"));

	//create entity for plane
	Ogre::Entity* ent = mgr->createEntity(nym+"Entity", "cube.mesh");
	rootNode->attachObject(ent);
	rootNode->scale(tile_dimensions.x/100.0f, tile_dimensions.y/100.0f, tile_dimensions.z/100.0f);
	switch(pID)
	{
		case 1:
			ent->setMaterialName("Examples/floor_texture_green");
			break;
		case 2:
			ent->setMaterialName("Examples/floor_texture_blue");
			break;
		case 3:
			ent->setMaterialName("Examples/floor_texture_purple");
			break;
		case 4:
			ent->setMaterialName("Examples/floor_texture_red");
			break;
	}
	ent->setCastShadows(false);
	rootNode->setPosition(position);
	rootNode->setVisible(false);

	mass = 0.0f;

	shape = new btBoxShape(btVector3(tile_dimensions.x/2.0f, tile_dimensions.y/2.0f, tile_dimensions.z/2.0f));
}