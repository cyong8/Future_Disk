#include "Target.h"


Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, btScalar height, btScalar radius, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Target";
	hit = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	// rootNode->attachObject(ent);

	// rootNode->scale();
	// rootNode->setPosition();
	// rootNode->translate();

	ent->setMaterialName("Examples/Robot");

//	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/2, dimensions.z/2));
}

