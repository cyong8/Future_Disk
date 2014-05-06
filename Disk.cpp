#include "Disk.h"
#include "Player.h"
#include "Target.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real dropToPlayer) 
	: GameObject(nym, mgr, sim)
{	
	/* 
		Sun Particle System from Ogre website:
			http://www.ogre3d.org/tikiwiki/tiki-index.php?page=ParticleExampleSun&structure=Cookbook
	*/
	needsOrientationUpdate = false;
	tailParticle[0] = mgr->createParticleSystem("Sun", "Examples/Sun");
	tailParticle[1] = mgr->createParticleSystem("RedSun", "Examples/RedSun");
	tailParticle[2] = mgr->createParticleSystem("GreenSun", "Examples/GreenSun");
	tailParticle[3] = mgr->createParticleSystem("CyanSun", "Examples/CyanSun");
	tailParticle[4] = mgr->createParticleSystem("WhiteSun", "Examples/WhiteSun");
	particleNode = rootNode->createChildSceneNode("Particle");
	particleNode->attachObject(tailParticle[0]);
	previousParticleSystem = 0;

	playerIDOfHolder = (int)dropToPlayer;
	printf("Initial Player Index = %d\n", playerIDOfHolder);
	
	// if (dropToPlayer >= 0.0f && dropToPlayer >= 1.0f)
	// 	initialPlayer = "Player1";
	// if (dropToPlayer == 1.0f)
	// 	initialPlayer = "Player2";

		
	powerUp = TARGET;

	Ogre::Vector3 position = Ogre::Vector3(0.0f, 0.0f, 0.0f);
	Ogre::Vector3 disk_dimensions = Ogre::Vector3(1.0f, 0.02f, 1.0f);

	particleNode->setVisible(true);
	
	typeName = "Disk";

	//BALL: Ogre::Entity* ent = mgr->createEntity(nym, "sphere.mesh"); // Create Entity; apply mesh
	Ogre::Entity* ent = mgr->createEntity(nym, "column.mesh");
	rootNode->attachObject(ent); // Attach disk to a scene node
	// Scale the disk to fit the world - we need the disk in the y-direction to be much smaller
	rootNode->scale(disk_dimensions.x/47.0f, disk_dimensions.y/442.0f, disk_dimensions.z/47.0f); //FOR DISK
	//rootNode->scale(disk_dimensions.x/200.0f, disk_dimensions.y/200.0f, disk_dimensions.z/200.0f);
	rootNode->setPosition(position.x, position.y, position.z); // Set the position of the disk
	
	ent->setMaterialName("Examples/Chrome"); // apply a material to the mesh

	shape = new btSphereShape(disk_dimensions.x/2.0f);
	mass = 0.1f;
	offWallRotation = false;
	
	playerLastThrew = NULL;
}
//-------------------------------------------------------------------------------------
void Disk::setThrownVelocity(btVector3 v)
{
	thrownVelocity = v;

	btTransform dTrans = body->getCenterOfMassTransform();
	Ogre::Vector3 dPos = Ogre::Vector3(rootNode->getPosition().x, rootNode->getPosition().y, 0.0f);
	Ogre::Vector3 vPos = Ogre::Vector3(v.getX(), v.getY(), 0.0f);
   	btQuaternion diskOrientation;

	Ogre::Radian angleOfNewRoll = dPos.angleBetween(vPos);
	rootNode->roll(-angleOfNewRoll);

	diskOrientation = btQuaternion(0.0f, 0.0f, rootNode->getOrientation().getRoll().valueRadians());
	dTrans.setRotation(diskOrientation);
	body->setCenterOfMassTransform(dTrans);
}
//-------------------------------------------------------------------------------------
btVector3 Disk::getThrownVelocity()
{
	return thrownVelocity;
}
//-------------------------------------------------------------------------------------
void Disk::createNewParticleSystem(int index)
{
    this->tailParticle[this->previousParticleSystem]->clear();
    this->particleNode->detachObject(this->tailParticle[this->previousParticleSystem]);
	this->particleNode->attachObject(this->tailParticle[index]);
	this->previousParticleSystem = index;
}
//-------------------------------------------------------------------------------------
bool Disk::activatePowerUp(powerUpType pType, Player* p)
{
    if (pType == EXPLOSIVE || pType == SPEED) 
    {
        powerUp = pType;
        if (powerUp == EXPLOSIVE && previousParticleSystem != 1)
            createNewParticleSystem(1);
        else if (powerUp == SPEED && previousParticleSystem != 2)
            createNewParticleSystem(2);
    }
    else if (pType == JUMPBOOST && p != NULL) 
    {
        p->increaseJump();
    }
    else if (pType == RESTORE) {
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------
void Disk::resetPowerUp()
{
    powerUp = TARGET;
    if (previousParticleSystem != 0)
        createNewParticleSystem(0);
}
//-------------------------------------------------------------------------------------
void Disk::setHolder(int pID)
{
	playerIDOfHolder = pID;
}
//-------------------------------------------------------------------------------------
int Disk::checkIDOfHolder()
{
	return playerIDOfHolder;
}
//-------------------------------------------------------------------------------------
void Disk::setPlayerLastThrew(Player* p)
{
    playerLastThrew = p;
}
//-------------------------------------------------------------------------------------
Player* Disk::getPlayerLastThrew()
{
    return playerLastThrew;
}
