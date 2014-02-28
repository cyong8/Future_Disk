#include "Target.h"
#include "Simulator.h"


Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position)
	: GameObject(nym, mgr, sim)
{
	typeName = "Target";
	hit = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	rootNode->attachObject(ent);

	rootNode->scale(dimensions.x/47.0f, dimensions.y/442.0f, dimensions.z/47.0f);
	
	//rootNode->setPosition(position);
	rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("leftwall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										,sim->getGameObject("rightwall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									   Ogre::Math::RangeRandom(sim->getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									   Ogre::Math::RangeRandom(sim->getGameObject("Ceiling")->getSceneNode()->getPosition().z/2
										,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().z));

	rootNode->pitch(Ogre::Degree(90));

	ent->setMaterialName("Examples/BlueChrome");

	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/10, dimensions.z/2));
	this->rootNode->showBoundingBox(true);
}

void Target::targetHit()
{
	hit = true;
}

void Target::resetHit()
{
	hit = false;
}

bool Target::isHit()
{
	return hit;
}