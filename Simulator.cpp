#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "Target.h"
#include "PlayerCamera.h"
#include "Disk.h"

Simulator::Simulator() 
{
	//collision configuration contains default setup for memory, collision setup.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a different dispatcher
	overlappingPairCache = new btDbvtBroadphase();
	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();
	//assign collision dispatcher to the collision configuration (Cody)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	
	dynamicsWorld->setGravity(btVector3(0,-9.8, 0));
	dynamicsWorld->debugDrawWorld();
	//keep track of the shapes, we release memory at exit
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
}

Simulator::~Simulator()
{
	delete(collisionConfiguration);
	delete(overlappingPairCache);
	delete(solver);
	delete(dynamicsWorld);
}

void Simulator::addObject (GameObject* o) 
{
	o->getBody()->setActivationState(DISABLE_DEACTIVATION);
	
	objList.push_back(o);
	//use default collision group/mask values (dynamic/kinematic/static)
	dynamicsWorld->addRigidBody(o->getBody());

	/* Set custom btRigidBody WRT specific GameObjects */
	if(o->typeName == "Player")
	{
		this->setPlayer((Player*)o);
		o->getBody()->setAngularFactor(btVector3(0,0,0));

		// Initialize PlayerCam Position now that you have the player and its CameraNode position
		this->player1Cam->initializePosition(((Player*)o)->getPlayerCameraNode()->_getDerivedPosition());
	}
	
	if(o->typeName == "Disk")
	{
		o->getBody()->setAngularFactor(btVector3(0,0,0));
		o->getBody()->setGravity(btVector3(0,0.0,0));
		o->getBody()->setRestitution(1);
		o->getBody()->setLinearVelocity(btVector3(5.0f, 5.0f, 1.0f));
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
	for (int i = 0; i < objList.size(); i++)
	{
		if (Ogre::StringUtil::match(objList[i]->getGameObjectName(), name, true))
		{
			objList.erase(objList.begin() + i);
		}
	}
}

// original stepSimulation is in btDiscreteDynamicsWorld
void Simulator::stepSimulation(const Ogre::Real elapseTime, int maxSubSteps, const Ogre::Real fixedTimestep)
{
	if(p1->checkHolding())
	{
		Ogre::LogManager::getSingletonPtr()->logMessage("\n\n\n__________HOLDING UPDATING_________\n\n\n\n");
		p1->getPlayerDisk()->getSceneNode()->setPosition(p1->getSceneNode()->getPosition());
		p1->getPlayerDisk()->getSceneNode()->needUpdate(true);
		// p1->getPlayerDisk()->getSceneNode()->translate(Ogre::Vector3(p1->getPlayerDimensions().x, 0, 0));
	}


	//do we need to update positions in simulator for dynamic objects?
	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);
	if (player1Cam)
	{
		player1Cam->update(elapseTime, p1->getPlayerCameraNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
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

		if (gA->typeName == "Player")
		{
			if (gB->typeName == "Disk")
			{
				if (((Player*)gA)->checkHolding() == false)
					((Player*)gA)->attachDisk((Disk*)gB);
			}
		}
		if (gB->typeName == "Player")
		{
			if (gA->typeName == "Disk")
			{
				if (((Player*)gB)->checkHolding() == false)
					((Player*)gB)->attachDisk((Disk*)gA);
			}
		}
		if (gA->typeName == "Target") // and other object was disk
		{
			if (gB->typeName == "Disk")
				((Target*)gA)->setTargetHit();
		}
		if (gB->typeName == "Target")
		{
			if (gA->typeName == "Disk")
				((Target*)gB)->setTargetHit();
		}

		//contactManifold->clearManifold();	
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