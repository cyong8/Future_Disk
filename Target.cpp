#include "Target.h"
#include "Simulator.h"


<<<<<<< HEAD
Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position, targetType tt, Ogre::Vector3 roomBounds)
=======
Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position)
>>>>>>> db56838525a784dd04a01464d38e679920442369
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

	//Ogre::Vector3 roomBounds = Ogre::Vector3(room->getWidth()/2, room->getHeight()/2, room->getFloorLength()/2);

	hit = false;

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	rootNode->attachObject(ent);

	rootNode->scale(dimensions.x/47.0f, dimensions.y/442.0f, dimensions.z/47.0f);
	
	//rootNode->setPosition(position);
<<<<<<< HEAD
	if (tt == POINT) {
	    rootNode->setPosition(Ogre::Math::RangeRandom(-roomBounds.x/2 + (1.0f/2.0f), roomBounds.x/2 - (1.0f/2.0f)), Ogre::Math::RangeRandom(-roomBounds.y/2 + (2.0f/3.0f), roomBounds.y/2 - (2.0f/3.0f)), Ogre::Math::RangeRandom(-roomBounds.z/2,roomBounds.z/2));
	    /*rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
=======
	if (nym != "Power" && nym != "Speed" && nym != "Shield" && nym != "Boost" && nym != "Jump" && nym != "Restore") {
	    rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
>>>>>>> db56838525a784dd04a01464d38e679920442369
										    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("Ceiling")->getSceneNode()->getPosition().z
										    ,sim->getGameObject("FarWall")->getSceneNode()->getPosition().z));*/
    }
    else {
        rootNode->setPosition(Ogre::Math::RangeRandom(-roomBounds.x/2 + (1.0f/2.0f), roomBounds.x/2 - (1.0f/2.0f)), Ogre::Math::RangeRandom(-roomBounds.y/2 + (2.0f/3.0f), roomBounds.y/2 - (2.0f/3.0f)), Ogre::Math::RangeRandom(-5.0f, 5.0f));
       /* rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									       Ogre::Math::RangeRandom(sim->getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									       Ogre::Math::RangeRandom(-5.0f, 5.0f));*/
    }
    
	rootNode->pitch(Ogre::Degree(90));

    typeName = "Target";

	if (nym == "Power") {
	    ent->setMaterialName("Examples/RedChrome");
    }
    else if (nym == "Speed") {
        ent->setMaterialName("Examples/GreenChrome");
    }
    else if (nym == "Shield") {
        ent->setMaterialName("Examples/CyanChrome");
    }
    else if (nym == "Boost") {
        ent->setMaterialName("Examples/YellowChrome");
    }
    else if (nym == "Jump") {
        ent->setMaterialName("Examples/MagentaChrome");
    }
    else if (nym == "Restore") {
        ent->setMaterialName("Examples/WhiteChrome");
    }
    else {
        ent->setMaterialName("Examples/BlueChrome");
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
