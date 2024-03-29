#include "Target.h"
#include "Simulator.h"
#include "Room.h"

Target::Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Room* rm, powerUpType ptype, int powerUpNumber)
	: GameObject(nym, mgr, sim)
{
	powerType = ptype;
    typeName = "Target";
    active = false;
    receiverID = -1;
    powerIndex = powerUpNumber;

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
    
	rootNode->pitch(Ogre::Degree(90));

	Ogre::Real posx, posy, posz;
    posx = Ogre::Math::RangeRandom(-rm->getWidth()/2.0f + dimensions.x, rm->getWidth()/2.0f - dimensions.x);
	posy = Ogre::Math::RangeRandom(rm->getFloorPositionY()/2.0f + dimensions.y, -rm->getFloorPositionY()/2.0f - dimensions.y);

	if (ptype == EXPLOSIVE) 
	{
		// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");	
	    ent->setMaterialName("Examples/RedChrome");
	}
    else if (ptype == SPEED) 
    {
    	// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");	
        ent->setMaterialName("Examples/GreenChrome");
    }
    else if (ptype == JUMPBOOST) 
    {
    	// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");	
        ent->setMaterialName("Examples/CyanChrome");
    }
    else if (ptype == RESTORE) 
    {
    	// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");	
        ent->setMaterialName("Examples/WhiteChrome");
    }
    else 
    {
    	// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "");	
        ent->setMaterialName("Examples/BlueChrome");
    }
    if (powerType != TARGET)
    {
		posz = Ogre::Math::RangeRandom(-rm->getGapSize()/2.0f, rm->getGapSize()/2.0f);
		rootNode->setVisible(false);
    }
    else
    {
    	// particleSystem = mgr->createParticleSystem(nym + Ogre::StringConverter::toString("_particle"), "Examples/Swarm");	
		posz = Ogre::Math::RangeRandom(-rm->getGapSize()/2.0f, -rm->getHeight()/2.0f);
	}

	// rootNode->attachObject(particleSystem);
	rootNode->setPosition(posx, posy, posz);

	shape = new btCylinderShape(btVector3(dimensions.x/2, dimensions.y/10, dimensions.z/2));
	mass = 0.1f;
}
//-------------------------------------------------------------------------------------
void Target::setActive(bool v)
{
	rootNode->setVisible(v);
	active = v;
}
//-------------------------------------------------------------------------------------
void Target::setReceiverID(int pID)
{
	receiverID = pID;
}
//-------------------------------------------------------------------------------------