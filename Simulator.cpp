#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Disk.h"

Simulator::Simulator(Ogre::SceneManager* mSceneMgr, Music* music) 
{
	score = 0;
	soundedJump = false;
	viewChangeP1 = false;
	viewChangeP2 = false;
	throwFlag = false;
	gameStart = false;
	player1CanCatch = true;
	previousWallHit = "NULL";

	gameMusic = music;
	sceneMgr = mSceneMgr;
	// initialize random number generate
    srand(time(0));

	//collision configuration contains default setup for memory, collision setup.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a different dispatcher
	overlappingPairCache = new btDbvtBroadphase();
	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	dynamicsWorld->setGravity(btVector3(0.0f,-9.8f, 0.0f));
	dynamicsWorld->debugDrawWorld();
	//keep track of the shapes, we release memory at exit
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
}
Simulator::~Simulator()
{
	gameMusic = NULL;
	delete(collisionConfiguration);
	delete(overlappingPairCache);
	delete(solver);
	delete(dynamicsWorld);
}

void Simulator::addObject (GameObject* o) 
{
	o->getBody()->setActivationState(DISABLE_DEACTIVATION);	
	objList.push_back(o); // Add the object to the list of object

	dynamicsWorld->addRigidBody(o->getBody());
	// Set custom btRigidBody WRT specific GameObjects 
	if(o->typeName == "Player")
	{
		setPlayer((Player*)o);
		o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		player1Cam->initializePosition(((Player*)o)->getPlayerCameraNode()->_getDerivedPosition(), ((Player*)o)->getPlayerSightNode()->_getDerivedPosition());
		player1Cam->setPlayer((Player*)o);
	}
	if(o->typeName == "Disk")
	{
		gameDisk = (Disk*)o;
		//o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		o->getBody()->setRestitution(1.0f);
		o->getBody()->setLinearVelocity(btVector3(15.0f, 15.0f, 4.0f));
		gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());
	}
	if(o->typeName == "Target")
	{
		if (o->checkReAddFlag()){
			((Target*)o)->resetHit();
			score = 10;

		}
		targetList.push_back((Target*)o);
	}
	if(o->typeName == "Wall")
	{
		o->getBody()->setRestitution(0.8f);
	}
}

GameObject* Simulator::getGameObject(Ogre::String name)
{
	for (int i = 0; i < objList.size(); i++)
	{
		if (Ogre::StringUtil::match(objList[i]->getGameObjectName(), name, true))
		{
			return objList[i];
		}
	}
	return NULL;
}

void Simulator::removeObject(Ogre::String name)
{
	if (getGameObject(name)->typeName == "Target")
	{
		for (int i = 0; i < targetList.size(); i++)
		{	
			if (Ogre::StringUtil::match(targetList[i]->getGameObjectName(), name, true))
			{
				targetList.erase(targetList.begin());	
			}
		}
	}	
	for (int i = 0; i < objList.size(); i++)
	{
		if (Ogre::StringUtil::match(objList[i]->getGameObjectName(), name, true))
		{
			dynamicsWorld->removeRigidBody(getGameObject(name)->getBody());
			getGameObject(name)->removeFromSimulator();
			objList.erase(objList.begin() + i);
		}
	}
}

// original stepSimulation is in btDiscreteDynamicsWorld
void Simulator::stepSimulation(const Ogre::Real elapseTime, int maxSubSteps, const Ogre::Real fixedTimestep)
{
	Ogre::Vector3 sightPosBeforeSim1;
	Ogre::Vector3 sightPosBeforeSim2;
	if (p1)
		// keep track of sight node position to reapply it after simulator is stepped 
		sightPosBeforeSim1 = p1->getPlayerSightNode()->getPosition(); 
	
	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);

	if (p1)
		p1->getPlayerSightNode()->setPosition(sightPosBeforeSim1);
	// update the rotation of the disk scene node
	//gameDisk->rotateOffWall();
	if (player1Cam)
		updatePlayerCamera(player1Cam, elapseTime);
	if (p1->checkHolding())
        performThrow(p1);
	else	// Speed disk back up in order to mimic inelasticity
	{	
		btVector3 currentDirection = gameDisk->getBody()->getLinearVelocity().normalized();
		gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(15.0f, 15.0f, 15.0f));
		
	}
}

void Simulator::parseCollisions(void)
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	int i;
	int groundCheck = false; //checking floor taking care of multiple collisions
	Player* colP;

	for (i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody* obA = static_cast<btRigidBody*>(contactManifold->getBody0());
		btRigidBody* obB = static_cast<btRigidBody*>(contactManifold->getBody1());
		
		OgreMotionState* OMSA = static_cast<OgreMotionState*>(obA->getMotionState());
		OgreMotionState* OMSB = static_cast<OgreMotionState*>(obB->getMotionState());

		GameObject* gA = OMSA->getGameObject();
		GameObject* gB = OMSB->getGameObject();

		if (gA->typeName == "Disk")
			handleDiskCollisions(gA, gB);
		else if (gB->typeName == "Disk")
			handleDiskCollisions(gB, gA);
		else if ((gA->typeName == "Player" && gB->getGameObjectName() == "Floor") || (gB->typeName == "Player" && gA->getGameObjectName() == "Floor"))
		{
			if (!groundCheck)
				groundCheck = true;
		}

		contactManifold->clearManifold();
	}
	if (!groundCheck)
		soundedJump = true;
	else
		p1->groundConstantSet = false;
	if (groundCheck && !gameStart)
		gameStart = true;

	if (soundedJump && groundCheck)	// played jumping sound, now check if he has hit the ground(landed)
	{
		//gameMusic->playCollisionSound("Player", "Ground"); // Not sure if I like this collision sound or if it's necessary
		soundedJump = false;
	}
}

void Simulator::setCamera(PlayerCamera* pcam)
{
	if (Ogre::StringUtil::match(pcam->name, "P1Cam", true))
		this->player1Cam = pcam;
	if (Ogre::StringUtil::match(pcam->name, "P2Cam", true))
		this->player2Cam = pcam;
}
void Simulator::setPlayer(Player* p)
{
	if (Ogre::StringUtil::match(p->getGameObjectName(), "Player1", true))
		this->p1 = p;
	if (Ogre::StringUtil::match(p->getGameObjectName(), "Player2", true))
		this->p2 = p;
}
PlayerCamera* Simulator::getPlayerCamera(Ogre::String name)
{
	if (Ogre::StringUtil::match(name, "P1Cam", true))
		return this->player1Cam;
	if (Ogre::StringUtil::match(name, "P2Cam", true))
		return this->player2Cam;
}
void Simulator::toggleViewChange(Ogre::String name)
{
	if (Ogre::StringUtil::match(name, "Player1", true))
		viewChangeP1 = !viewChangeP1;
	if (Ogre::StringUtil::match(name, "Player2", true))
		viewChangeP2 = !viewChangeP2;
}
void Simulator::setThrowFlag()
{
	throwFlag = !throwFlag;
}
void Simulator::performThrow(Player* p)
{
   	Disk *d = p->getPlayerDisk();	
   	btQuaternion diskOrientation;
 	btTransform transform;

	if (throwFlag) // Add disk back to simulator and it will take care of throw velocity
    {	
    	Ogre::Vector3 toParentPosition = d->getSceneNode()->_getDerivedPosition();
    	transform = d->getBody()->getCenterOfMassTransform();
    	p->setHolding();
		// rotate disk node here

		/* Set the disk direction vector to be the same as the player's sight node vector */
		Ogre::Vector3 diskDirection = p->getPlayerSightNode()->getPosition().normalisedCopy();
		
		/* The new disk direction is along player's orientation */
		diskDirection = p->getSceneNode()->getOrientation() * diskDirection;
		
		Ogre::Vector3 sightNodePosition = Ogre::Vector3(p->getPlayerSightNode()->getPosition().x, p->getPlayerSightNode()->getPosition().y, 0);
		Ogre::Vector3 playerPosition = Ogre::Vector3(p->getSceneNode()->getPosition().x, p->getSceneNode()->getPosition().y, 0);

		Ogre::Radian angleOfNewPitch = playerPosition.angleBetween(sightNodePosition);
		d->getSceneNode()->pitch(-angleOfNewPitch);
				
		diskOrientation = btQuaternion(0, d->getSceneNode()->getOrientation().getPitch().valueRadians(), 0);
        transform.setRotation(diskOrientation);
		d->getBody()->setCenterOfMassTransform(transform);

		//o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		d->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		d->getBody()->setRestitution(1.0f);
		d->getBody()->setLinearVelocity(btVector3(15.0f, 15.0f, 15.0f) * btVector3(diskDirection.x*1.3f, diskDirection.y*1.3f, diskDirection.z*1.3f));

		gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());

		p->getSceneNode()->removeChild(d->getSceneNode()); // detach disk from parent
		sceneMgr->getRootSceneNode()->addChild(d->getSceneNode()); // attach disk to world (root)
		d->getSceneNode()->setPosition(toParentPosition); // retain the same global position

		throwFlag = false;
		// trying to fix player immediately catching disk after throw
		player1CanCatch = false; 
    }
    else // Update position relative to the Player
    {
    	Ogre::Vector3 dpos;
    	float newDiskZ = -p->getPlayerDimensions().z;
    	dpos = p->getSceneNode()->getOrientation() * Ogre::Vector3(0.0f, 0.0f, newDiskZ);
		d->getSceneNode()->_setDerivedPosition(dpos + p->getSceneNode()->getPosition());

		Ogre::Vector3 dpos_derived = d->getSceneNode()->_getDerivedPosition();
		diskOrientation = btQuaternion(0, 0, 0);
		transform = btTransform(diskOrientation, btVector3(dpos_derived.x, dpos_derived.y, dpos_derived.z));
		d->getBody()->setCenterOfMassTransform(transform);
    }
}
int Simulator::tallyScore(void)
{
	int tmpScore = score;
	score = 0;
	return tmpScore;
}
void Simulator::handleDiskCollisions(GameObject* disk, GameObject* o)
{
	// Wall
	if (o->typeName == "Wall")
	{
		if (previousWallHit == "NULL")
		{
			adjustDiskOrientation(gameDisk, disk->getBody()->getLinearVelocity());
			previousWallHit = o->getGameObjectName();
			gameMusic->playCollisionSound("Disk", "Wall");
		}
		else if (previousWallHit != o->getGameObjectName())
		{
			adjustDiskOrientation(gameDisk, disk->getBody()->getLinearVelocity());
			previousWallHit = o->getGameObjectName();	
			gameMusic->playCollisionSound("Disk", "Wall");
		}
		if (!player1CanCatch)
			player1CanCatch = true;
	}
	// Player
	else if (o->typeName == "Player")
	{
		if (((Player*)o)->checkHolding() == false && player1CanCatch)
		{
			((Player*)o)->attachDisk((Disk*)disk);
			gameMusic->playCollisionSound("Disk", "Player");
		}
	}
	// Target
	else if (o->typeName == "Target")
	{
		if (((Target*)o)->isHit() == false)
		{
			((Target*)o)->targetHit();
			removeObject(o->getGameObjectName());
			// The 47.0f value is the x-width and y-height of the disk
			o->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
								,getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
							   Ogre::Math::RangeRandom(getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
								,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
							   Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z
								,getGameObject("FarWall")->getSceneNode()->getPosition().z));
			o->addToSimulator();
			gameMusic->playCollisionSound("Disk", "Target");
		}
	}
}

void Simulator::adjustDiskOrientation(Disk* d, btVector3 currVelocity)
{
	if(currVelocity != d->getOldVelocity())
	{
		Ogre::Vector3 currRoll = Ogre::Vector3(currVelocity.getX(), currVelocity.getY(), 0);
		Ogre::Vector3 oldRoll = Ogre::Vector3(d->getOldVelocity().getX(), d->getOldVelocity().getY(), 0);
		Ogre::Vector3 currPitch = Ogre::Vector3(0, currVelocity.getY(), currVelocity.getZ());
		Ogre::Vector3 oldPitch = Ogre::Vector3(0, d->getOldVelocity().getY(), d->getOldVelocity().getZ());

		btTransform trans = d->getBody()->getCenterOfMassTransform();
		btQuaternion quat;

		//adjust pitch 
		Ogre::Radian angleOfNewPitch = currPitch.angleBetween(oldPitch);
		angleOfNewPitch = -(Ogre::Degree(180.0f).valueRadians() - angleOfNewPitch * 2.0f);
		d->getSceneNode()->pitch(angleOfNewPitch);
		//adjust roll
		Ogre::Radian angleOfNewRoll = currRoll.angleBetween(oldRoll);
		d->getSceneNode()->roll(-(Ogre::Degree(180.0f).valueRadians() - angleOfNewRoll * 2.0f));

		quat = btQuaternion(d->getSceneNode()->getOrientation().getRoll().valueRadians(), d->getSceneNode()->getOrientation().getPitch().valueRadians(), 0);
        trans.setRotation(quat);
		d->getBody()->setCenterOfMassTransform(trans);
		d->setOldVelocity(currVelocity);
	}
}

void Simulator::handlePlayerCollisions(GameObject* cPlayer, GameObject* o)
{

}
void Simulator::updatePlayerCamera(PlayerCamera* cam, const Ogre::Real elapseTime)
{
	if (cam->name == "P1Cam")
	{
		if (viewChangeP1) // View was toggled; now check what view it needs to be changed to
		{
			toggleViewChange(p1->getGameObjectName()); // want to set toggle flag back since you are now either entering or leaving Aim View

			if(player1Cam->isInAimMode()) // Go into Aim view
				player1Cam->initializePosition(((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			

			else // Return from Aim view
				player1Cam->initializePosition(p1->getPlayerCameraNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
		}
		else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
		{
			if (player1Cam->isInAimMode())
				player1Cam->update(elapseTime, ((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			else
				player1Cam->update(elapseTime, p1->getPlayerCameraNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());			
		}
	}
	if (cam->name == "P2Cam")
	{
		if (viewChangeP1) // View was toggled; now check what view it needs to be changed to
		{
			toggleViewChange(p2->getGameObjectName()); // want to set toggle flag back since you are now either entering or leaving Aim View

			if(player1Cam->isInAimMode()) // Go into Aim view
				player1Cam->initializePosition(((GameObject*)p2)->getSceneNode()->_getDerivedPosition(), p2->getPlayerSightNode()->_getDerivedPosition());
			else // Return from Aim view
				player1Cam->initializePosition(p2->getPlayerCameraNode()->_getDerivedPosition(), p2->getPlayerSightNode()->_getDerivedPosition());
		}
		else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
		{
			if (player1Cam->isInAimMode())
				player1Cam->update(elapseTime, ((GameObject*)p2)->getSceneNode()->_getDerivedPosition(), p2->getPlayerSightNode()->_getDerivedPosition());
			else
				player1Cam->update(elapseTime,p2->getSceneNode()->_getDerivedPosition(), p2->getPlayerSightNode()->_getDerivedPosition());
		}
	}
}
bool Simulator::checkGameStart()
{
	return gameStart;
}