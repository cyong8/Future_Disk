#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Disk.h"

Simulator::Simulator(Ogre::SceneManager* mSceneMgr) 
{
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
		// Initialize PlayerCam Position now that you have the player and its CameraNode position
		player1Cam->initializePosition(Ogre::Vector3(0.0f, 1.2f, 12.5f) + p1->getSceneNode()->getPosition(), ((Player*)o)->getPlayerSightNode()->getPosition());
		player1Cam->setPlayer((Player*)o);
	}
	if(o->typeName == "Disk")
	{
		if (!o->checkReAddFlag())
		{
			o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setRestitution(1);
			o->getBody()->setLinearVelocity(btVector3(8.0f, 8.0f, 4.0f));
		}
		else 
		{
			Ogre::Vector3 diskDirection = p1->getPlayerSightNode()->getPosition().normalisedCopy();
			o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
			o->getBody()->setRestitution(1);
			o->getBody()->setLinearVelocity(btVector3(15.0f, 15.0f, 15.0f) * btVector3(diskDirection.x, diskDirection.y, diskDirection.z));
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
        	//float newDiskX = p1->getPlayerSightNode()->_getDerivedPosition().x - p1->getPlayerDisk()->getSceneNode()->getPosition().x;
        	//float newDiskY = p1->getPlayerSightNode()->_getDerivedPosition().y - p1->getPlayerDisk()->getSceneNode()->getPosition().y;
        	float newDiskZ = -p1->getPlayerDimensions().z;

			p1->getPlayerDisk()->getSceneNode()->_setDerivedPosition(Ogre::Vector3(0.0f, 0.0f, newDiskZ) + p1->getSceneNode()->getPosition());
        }
	}
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

		// ********** Attach Disk *************
		if (gA->typeName == "Player")
		{
			if (gB->typeName == "Disk")
			{
				if (((Player*)gA)->checkHolding() == false)
				{
					((Player*)gA)->attachDisk((Disk*)gB);
					removeObject("Disk");
				}
			}
		}
		if (gB->typeName == "Player")
		{
			if (gA->typeName == "Disk")
			{
				if (((Player*)gB)->checkHolding() == false)
				{
					((Player*)gB)->attachDisk((Disk*)gA);
					removeObject("Disk");
				}
			}
		}
		// ********** Hit Targets *************
		if (gA->typeName == "Target") // and other object was disk
		{
			if (gB->typeName == "Disk")
			{
				if (((Target*)gA)->isHit() == false)
				{
					((Target*)gA)->targetHit();
					removeObject("Target");
					gA->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("rightwall")->getSceneNode()->getPosition().x
										,getGameObject("leftwall")->getSceneNode()->getPosition().x), 
									   Ogre::Math::RangeRandom(getGameObject("Floor")->getSceneNode()->getPosition().y
										,getGameObject("Ceiling")->getSceneNode()->getPosition().y), 
									   Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z/2
										,getGameObject("Ceiling")->getSceneNode()->getPosition().z));
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
					removeObject("Target");
					gB->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("rightwall")->getSceneNode()->getPosition().x
										,getGameObject("leftwall")->getSceneNode()->getPosition().x), 
									   Ogre::Math::RangeRandom(getGameObject("Floor")->getSceneNode()->getPosition().y
										,getGameObject("Ceiling")->getSceneNode()->getPosition().y), 
									   Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z/2
										,getGameObject("Ceiling")->getSceneNode()->getPosition().z));
					gB->addToSimulator();
				}
			}
		}
		contactManifold->clearManifold();
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