#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer) 
	: GameObject(nym, mgr, sim)
{
	/*  Added a dropToPlayer attribute that will randomly decide who gets to start with the disk
		This is not implemented since we are in 1-player mode.
	*/
	Ogre::Vector3 position = Ogre::Vector3(-3.0f, 0.0f, 0.0f);
	Ogre::Vector3 disk_dimensions = Ogre::Vector3(0.5f, 0.01f, 0.5f);

	diskDirection = Ogre::Vector3(0.0f, 0.0f, -1.0f);
	
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
	offWallRotation = false;
}
bool Disk::checkOffWallRotation()
{
	return offWallRotation;
}
void Disk::setRotateOffWall()
{
	offWallRotation = true;
}
void Disk::rotateOffWall()
{
	// perform rotation of sceneNode to the angle v
	btVector3 lv = body->getLinearVelocity();
	Ogre::Vector3 velocityDirection = Ogre::Vector3(lv.x(), lv.y(), lv.z()).normalisedCopy(); //direction of the velocity

	Ogre::Quaternion diskRoll = diskDirection.getRotationTo(velocityDirection);
	rootNode->setOrientation(diskRoll);	
	diskDirection = velocityDirection;
	offWallRotation = false;
}