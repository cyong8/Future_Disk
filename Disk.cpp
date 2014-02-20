#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer) 
	: GameObject(nym, mgr, sim)
{
	/*  Added a dropToPlayer attribute that will randomly decide who gets to start with the disk
		This is not implemented since we are in 1-player mode.
	*/
	Ogre::Vector3 disk_dimensions = Ogre::Vector3(1.0f, 1.0f, 0.1f);
	typeName = "Disk";
	heldBy = NULL;

	double scale = 0.1;
	// Creates a Ogre::Entity for the player using a mesh of our choice
	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	// Attach disk to a scene node
	rootNode->attachObject(ent);
	// Scale the disk to fit the world - we need the disk in the y-direction to be much smaller
	rootNode->scale(disk_dimensions.x/47.0f, disk_dimensions.y/442.0f, disk_dimensions.z/47.0f);
	// Set the position of the disk
	rootNode->setPosition(position.x, position.y, position.z);

	ent->setMaterialName("Examples/Droplet");

	// Set collision shape for Bullet
	btCylindershape(disk_dimensions.x/2, disk_dimensions.y/2, disk_dimensions.z/2);
	mass = 0.1f;
}


void Disk::setPlayer(Player* p)
{
	heldBy = p;
}

