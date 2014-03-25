#include "Target.h"
#include "Simulator.h"


Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position, targetType tt)
	: GameObject(nym, mgr, sim)
{
	/* 
		Explosion Particle System from Ogre Website:
			http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Explosion&structure=Cookbook
	*/
	/*ParticleSystem* particleSystem = sceneManager->createParticleSystem("explosions", "explosionTemplate");
	particleSystem->fastForward(1.0); 	// fast forward 1 second  to the point where the particle has been emitted
	sceneNode->attachObject(particleSystem); // attach the particle system to a scene node
*/

	hit = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	rootNode->attachObject(ent);

	rootNode->scale(dimensions.x/47.0f, dimensions.y/442.0f, dimensions.z/47.0f);
	
	//rootNode->setPosition(position);
	if (tt == POINT) {
	    rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("Ceiling")->getSceneNode()->getPosition().z
										    ,sim->getGameObject("FarWall")->getSceneNode()->getPosition().z));
    }
    else {
        rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									       Ogre::Math::RangeRandom(-5.0f, 5.0f));
    }
    
	rootNode->pitch(Ogre::Degree(90));

    if (tt == POINT) {
    	typeName = "Target";
    	ent->setMaterialName("Examples/BlueChrome");
	}
	else if (tt == POWER) {
	    typeName = "Power";
	    ent->setMaterialName("Examples/RedChrome");
    }
    else if (tt == SPEED) {
        typeName = "Speed";
        ent->setMaterialName("Examples/GreenChrome");
    }
    else if (tt == SHIELD) {
        typeName = "Shield";
        ent->setMaterialName("Examples/CyanChrome");
    }
    else if (tt == BOOST) {
        typeName = "Boost";
        ent->setMaterialName("Examples/YellowChrome");
    }
    else if (tt == RESTORE) {
        typeName = "Restore";
        ent->setMaterialName("Examples/WhiteChrome");
    }

	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/10, dimensions.z/2));
}
//-------------------------------------------------------------------------------------
void Target::targetHit()
{
	hit = true;
}
//-------------------------------------------------------------------------------------
void Target::resetHit()
{
	hit = false;
}
//-------------------------------------------------------------------------------------
bool Target::isHit()
{
	return hit;
}
