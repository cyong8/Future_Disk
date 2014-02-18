#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real height, Ogre::Real radius, Ogre::Vector3 position) 
	: GameObject(nym, mgr, sim)
{
	typeName = "Disk";
	heldBy = NULL;

	double scale = 0.1;
	// Creates a Ogre::Entity for the player using a mesh of our choice
	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	// Attach disk to a scene node
	rootNode->attachObject(ent);
	// Scale the disk to fit the world - we need the disk in the y-direction to be much smaller
	// /rootNode->scale(1.0f/worldScale, 1.0f/(worldScale * 10), 1.0f/worldScale);
	// Set the position of the disk
	rootNode->setPosition(position);
	// Translate the disk - possibly not necesssary? (Kaitlin)
	rootNode->translate(0,0.1,0);

	//ent->setMaterialName("Examples/Robot");

	// Set collision shape for Bullet
	//btCylindershape(x/2,y/2,z/2);
	// Set mass of player
	//mass = 0.1f;
}


void Disk::setPlayer(Player* p)
{
	heldBy = p;
}

