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

	viewChangeP1 = false;
	viewChangeP2 = false;
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
	// Put the object back in the simulator
	o->getBody()->setActivationState(DISABLE_DEACTIVATION);
	
	// Add the object to the list of object
	objList.push_back(o);

	//use default collision group/mask values (dynamic/kinematic/static)
	dynamicsWorld->addRigidBody(o->getBody());

	// Set custom btRigidBody WRT specific GameObjects 
	if(o->typeName == "Player")
	{
		setPlayer((Player*)o);
		o->getBody()->setAngularFactor(btVector3(0,0,0));

		// Initialize PlayerCam Position now that you have the player and its CameraNode position
		player1Cam->initializePosition(Ogre::Vector3(0.0f, 5.0f, 20.0f), ((Player*)o)->getPlayerSightNode()->getPosition());
		player1Cam->setPlayer((Player*)o);
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
				// player1Cam->getMCamera()->setPosition(p1->getSceneNode()->getPosition());
				// player1Cam->update(elapseTime, p1->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			}
			else // Return from Aim view
			{
				player1Cam->initializePosition(Ogre::Vector3(0.0f, 2.0f, 20.0f), p1->getPlayerSightNode()->_getDerivedPosition());
			}
		}
		else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
		{
     		//player1Cam->getMCamera()->setPosition(Ogre::Vector3(0,5,20)); 
			if (player1Cam->isInAimMode())
			{
				player1Cam->update(elapseTime, ((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			}
			else
				player1Cam->update(elapseTime, Ogre::Vector3(0.0f, 2.0f, 20.0f), p1->getPlayerSightNode()->_getDerivedPosition());
		}
	}

	if(p1->checkHolding()) // move the ball into the players hand
		p1->getPlayerDisk()->getSceneNode()->translate(Ogre::Vector3(p1->getPlayerDimensions().x/2, 0.0f, 0.0f), Ogre::Node::TS_WORLD);

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

		// ********** Hit Targets *************
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