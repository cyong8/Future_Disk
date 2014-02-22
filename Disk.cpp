#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer) 
	: GameObject(nym, mgr, sim)
{
	/*  Added a dropToPlayer attribute that will randomly decide who gets to start with the disk
		This is not implemented since we are in 1-player mode.
	*/
	Ogre::Vector3 position = Ogre::Vector3(-0.5f, 0.7f, 0.5f);
	Ogre::Vector3 disk_dimensions = Ogre::Vector3(0.5f, 0.01f, 0.5f);
	
	typeName = "Disk";

	double scale = 0.1;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh"); // Create Entity; apply mesh
	rootNode->attachObject(ent); // Attach disk to a scene node
	// Scale the disk to fit the world - we need the disk in the y-direction to be much smaller
	rootNode->scale(disk_dimensions.x/47.0f, disk_dimensions.y/442.0f, disk_dimensions.z/47.0f);
	rootNode->setPosition(position.x, position.y, position.z); // Set the position of the disk

	ent->setMaterialName("Examples/Chrome"); // apply a material to the mesh

	shape = new btSphereShape(disk_dimensions.x/2.0f); // Sphere shape similar to project1
	mass = 0.1f;
}

// Function to update the orientation of the SceneNode on collision with a wall
void Disk::updateDiskSNodeOrientation(Ogre::Vector3 v)
{

}

void Disk::updateDiskPosition(Ogre::Vector3 v)
{
	// update position in Ogre
	this->getSceneNode()->setPosition(v);
	// update position in Bullet
	this->updateTransform();
}