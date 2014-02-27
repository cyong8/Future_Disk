#include "Target.h"


Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Target";
	hit = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	rootNode->attachObject(ent);

	rootNode->scale(dimensions.x/100.0f, dimensions.y/100.0f, dimensions.z/100.0f);
	
	rootNode->setPosition(position);
	rootNode->pitch(Ogre::Degree(90));

	ent->setMaterialName("Examples/BumpyMetal");

	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/10, dimensions.z/2));
	this->rootNode->showBoundingBox(true);
}

void Target::setTargetHit()
{
	hit = !hit;
}

bool Target::isHit()
{
	return hit;
}