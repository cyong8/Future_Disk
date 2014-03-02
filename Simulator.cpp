#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Disk.h"

Simulator::Simulator(Ogre::SceneManager* mSceneMgr, Music* music) 
{
	onFloor = false;
	gameMusic = music;
	previousWallHit = "NULL";
	soundedJump = true;

	// initialize random number generate
    srand(time(0));

	sceneMgr = mSceneMgr;
	//collision configuration contains default setup for memory, collision setup.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a different dispatcher
	overlappingPairCache = new btDbvtBroadphase();
	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	dynamicsWorld->setGravity(btVector3(0,-9.8, 0));
	dynamicsWorld->debugDrawWorld();
	//keep track of the shapes, we release memory at exit
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	viewChangeP1 = false;
	viewChangeP2 = false;
	throwFlag = false;
}

/*
	Simulator destructor class
*/
Simulator::~Simulator()
{
	gameMusic = NULL;
	delete(collisionConfiguration);
	delete(overlappingPairCache);
	delete(solver);
	delete(dynamicsWorld);
}

/*
	Add a GameObject to the simulator
*/
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
		player1Cam->initializePosition(Ogre::Vector3(0.0f, 1.2f, 12.5f) + p1->getSceneNode()->getPosition(), ((Player*)o)->getPlayerSightNode()->getPosition());
		player1Cam->setPlayer((Player*)o);
	}
	if(o->typeName == "Disk")
	{
		if (!o->checkReAddFlag())
		{
			gameDisk = (Disk*)o;
			//o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setRestitution(1);
			o->getBody()->setLinearVelocity(btVector3(15.0f, 15.0f, 4.0f));
			gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());
		}
		else 
		{
			Ogre::Vector3 diskDirection = p1->getPlayerSightNode()->getPosition().normalisedCopy();
			//o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setRestitution(1);
			o->getBody()->setLinearVelocity(btVector3(15.0f, 15.0f, 15.0f) * btVector3(diskDirection.x*1.3f, diskDirection.y*1.3f, diskDirection.z*1.3f));
			gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());
		}
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
		o->getBody()->setRestitution(.8);
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
	//do we need to update positions in simulator for dynamic objects?
	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);

	// update the rotation of the disk scene node
	//gameDisk->rotateOffWall();
	if (player1Cam)
	{
		if (viewChangeP1) // View was toggled; now check what view it needs to be changed to
		{
			toggleViewChange("Player1"); // want to set toggle flag back since you are now either entering or leaving Aim View
			if(player1Cam->isInAimMode()) // Go into Aim view
			{
				player1Cam->initializePosition(((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			}
			else // Return from Aim view
			{
				player1Cam->initializePosition(Ogre::Vector3(0.0f, 1.2f, 12.5f) + p1->getSceneNode()->getPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			}
		}
		else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
		{
			if (player1Cam->isInAimMode())
			{
				player1Cam->update(elapseTime, ((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			}
			else
				player1Cam->update(elapseTime, Ogre::Vector3(0.0f, 1.2f, 12.5f) + p1->getSceneNode()->getPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
		}
	}
	if (p1->checkHolding())
	{
        if (throwFlag) // Impart throw velocity along SightNode direction
        {	
        	Ogre::Vector3 toParentPosition = p1->getPlayerDisk()->getSceneNode()->_getDerivedPosition();
        	p1->setHolding();
			// rotate disk node here
			p1->getSceneNode()->removeChild(p1->getPlayerDisk()->getSceneNode()); // detach disk from parent
			sceneMgr->getRootSceneNode()->addChild(p1->getPlayerDisk()->getSceneNode()); // attach disk to world (root)
			p1->getPlayerDisk()->getSceneNode()->setPosition(toParentPosition); 
        	p1->getPlayerDisk()->addToSimulator(); // Add the Disk back into the Simulator 
			throwFlag = false;
        }
        else // Move to front of player
        {
        	float newDiskZ = -p1->getPlayerDimensions().z;

			p1->getPlayerDisk()->getSceneNode()->_setDerivedPosition(Ogre::Vector3(0.0f, 0.0f, newDiskZ) + p1->getSceneNode()->getPosition());
        }
	}
	else 
	{	
		// Speed disk back up in order to mimic inelasticity
		btVector3 currentDirection = gameDisk->getBody()->getLinearVelocity().normalized();
		gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(15.0f, 15.0f, 15.0f));
	}


/*	if (getGameObject("Disk") != NULL) // Rotate the SceneNode to mimic Disk-Wall collisions
    {
        // ((Disk*)getGameObject("Disk"))->rotateOffWall();
        // /((Disk*)getGameObject("Disk"))->updateTransform();
        Disk* tempDisk = ((Disk*)getGameObject("Disk"));
        // removeObject("Disk");
        btVector3 lv = tempDisk->getBody()->getLinearVelocity();
		Ogre::Vector3 velocityDirection = Ogre::Vector3(lv.x(), lv.y(), lv.z()).normalisedCopy(); //direction of the velocity

		Ogre::Quaternion diskRoll = tempDisk->diskDirection.getRotationTo(velocityDirection);
		tempDisk->getSceneNode()->_setDerivedOrientation(diskRoll);	
		tempDisk->diskDirection = velocityDirection;
		tempDisk->updateTransform();
		tempDisk->resetRotateOffWall();
		// tempDisk->addToSimulator();
    }*/
}

void Simulator::setHitFlags(void)
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	int i;
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
		{
			if (gB->typeName == "Wall")
			{
				if (previousWallHit == "NULL")
				{
					gameMusic->playCollisionSound("Disk", "Wall");
					previousWallHit = gB->getGameObjectName();
				}
				else if (previousWallHit != gB->getGameObjectName())
				{
					gameMusic->playCollisionSound("Disk", "Wall");
					previousWallHit = gB->getGameObjectName();	
				}
			}
		}
		if (gB->typeName == "Disk")
		{
			if (gA->typeName == "Wall")
			{
				if (previousWallHit == "NULL")
				{
					gameMusic->playCollisionSound("Disk", "Wall");
					previousWallHit = gA->getGameObjectName();
				}
				else if (previousWallHit != gA->getGameObjectName())
				{
					gameMusic->playCollisionSound("Disk", "Wall");
					previousWallHit = gA->getGameObjectName();	
				}
			
			}
		}
		

		// ********** Attach Disk *************
		if (gA->typeName == "Player")
		{
			if (gB->typeName == "Disk")
			{
				if (((Player*)gA)->checkHolding() == false)
				{
					((Player*)gA)->attachDisk((Disk*)gB);
					gameMusic->playCollisionSound("Disk", "Player");
					removeObject("Disk");
				}
			}
			if (gB->getGameObjectName() == "Floor" && !onFloor)
			{
				onFloor = true;
			}
		}
		if (gB->typeName == "Player")
		{
			if (gA->typeName == "Disk")
			{
				if (((Player*)gB)->checkHolding() == false)
				{
					((Player*)gB)->attachDisk((Disk*)gA);
					gameMusic->playCollisionSound("Disk", "Player");
					removeObject("Disk");
				}
			}
			if (gA->getGameObjectName() == "Floor" && !onFloor)
			{
				onFloor = true;
			}
		}
													// ********** Rotate Disk *************
													// if (gA->typeName == "Disk")
													// {
													// 	if (gB->typeName == "Wall")
													// 	{
													// 		if (((Disk*)gA)->checkOffWallRotation() == false)
													// 		{
													// 			((Disk*)gA)->setRotateOffWall();
													// 		}
													// 	}
													// }
													// if (gB->typeName == "Disk")
													// {
													// 	if (gA->typeName == "Wall")
													// 	{
													// 		if (((Disk*)gB)->checkOffWallRotation() == false)
													// 		{
													// 			((Disk*)gB)->setRotateOffWall();	
													// 		}
													// 	}
													// }

		// ********** Hit Targets *************
		if (gA->typeName == "Target") 
		{
			if (gB->typeName == "Disk") // and other object was disk
			{
				if (((Target*)gA)->isHit() == false)
				{
					((Target*)gA)->targetHit();
					gameMusic->playCollisionSound("Disk", "Target");
					removeObject(gA->getGameObjectName());
					// The 47.0f value is the x-width and y-height of the disk
					gA->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("leftwall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										,getGameObject("rightwall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									   Ogre::Math::RangeRandom(getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									   Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z
										,getGameObject("backwall")->getSceneNode()->getPosition().z));
					gA->addToSimulator();
				}
			}
		}
		if (gB->typeName == "Target")
		{
			if (gA->typeName == "Disk")
			{
				if (((Target*)gB)->isHit() == false)
				{
					((Target*)gB)->targetHit();
					gameMusic->playCollisionSound("Disk", "Target");
					removeObject(gB->getGameObjectName());
					// The 47.0f value is the x-width and y-height of the disk
					gB->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("leftwall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
										,getGameObject("rightwall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
									   Ogre::Math::RangeRandom(getGameObject("Floor")->getSceneNode()->getPosition().y + (2.0f/3.0f)
										,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
									   Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z
										,getGameObject("backwall")->getSceneNode()->getPosition().z));
					gB->addToSimulator();
				}
			}
		}
		contactManifold->clearManifold();
	}
	if (!onFloor && !soundedJump)
	{
		soundedJump = true;
	}
	if(onFloor && soundedJump)
	{
		gameMusic->playCollisionSound("Player", "Ground");
		soundedJump = false;
	}
}
void Simulator::setCamera(PlayerCamera* pcam)
{
	if (Ogre::StringUtil::match(pcam->name, "P1_cam", true))
		this->player1Cam = pcam;
	if (Ogre::StringUtil::match(pcam->name, "P2_cam", true))
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
	if (Ogre::StringUtil::match(name, "P1_cam", true))
		return this->player1Cam;
	if (Ogre::StringUtil::match(name, "P2_cam", true))
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
int Simulator::tallyScore(void)
{
	int tmpScore = score;
	score = 0;
	return tmpScore;
}
bool Simulator::allowMovementCheck(void)
{
	return allowMovement;
}
bool Simulator::checkOnFloor()
{
	return onFloor;
}
void Simulator::resetOnFloor()
{
	onFloor = false;
}