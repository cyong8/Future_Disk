#include "Target.h"
#include "Simulator.h"

Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 position) /* Position parameter is not used */
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

	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");

	rootNode->attachObject(ent);

	rootNode->scale(dimensions.x/47.0f, dimensions.y/442.0f, dimensions.z/47.0f);

	if(sim != NULL)
	{
		/* There is no client11 tile */
		// if (nym != "Power" && nym != "Speed" && nym != "Jump" && nym != "Restore") {
		//     rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
		// 									    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
		// 								       Ogre::Math::RangeRandom(sim->getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
		// 									    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
		// 								       Ogre::Math::RangeRandom(sim->getGameObject("Ceiling")->getSceneNode()->getPosition().z
		// 									    ,sim->getGameObject("FarWall")->getSceneNode()->getPosition().z));
	 //    }
	 //    else {
	 //       rootNode->setPosition(Ogre::Math::RangeRandom(sim->getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
		// 									    ,sim->getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
		// 								       Ogre::Math::RangeRandom(sim->getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
		// 									    ,sim->getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
		// 								       Ogre::Math::RangeRandom(-5.0f, 5.0f));
  //   	}
    }
    
	rootNode->pitch(Ogre::Degree(90));

	Ogre::Real posx, posy, posz;

	posx = Ogre::Math::RangeRandom(-position.x, position.x);
	posy = Ogre::Math::RangeRandom(0, position.y);
	posz = Ogre::Math::RangeRandom(-position.y, position.z);

	rootNode->setPosition(posx, posy, posz);

    typeName = "Target";

	if (nym == "Power") {
	    ent->setMaterialName("Examples/RedChrome");
    }
    else if (nym == "Speed") {
        ent->setMaterialName("Examples/GreenChrome");
    }
    else if (nym == "Jump") {
        ent->setMaterialName("Examples/CyanChrome");
    }
    else if (nym == "Restore") {
        ent->setMaterialName("Examples/WhiteChrome");
    }
    else {
        ent->setMaterialName("Examples/BlueChrome");
    }

	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/10, dimensions.z/2));
}
