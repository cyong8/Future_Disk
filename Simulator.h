#ifndef __Simulator_h_
#define __Simulator_h_

#include "GameObject.h"

class Simulator {
	protected:
		btDefaultCollissionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btConstraintSolver* mConstraintsolver;
		//btCollisionWorld* mWorld;
		Ogre::SceneManager* sceneMgr;
		std::deque<GameObject*> objList;

	public:
		Simulator();
		~Simulator();
		void addObject(GameObject* o);
		bool removeObject(GameObject* o);
		void stepSimulation(const Ogre::Real elapsedTime, 
				int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
};

Simulator::Simulator() {
	//collision configuration contains default setup for memory, collision setup.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a different dispatcher
	overlappingPairCache = new btDbvtBroadphase();
	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,-0.098, 0));
	//keep track of the shapes, we release memory at exit
	//make sure to re-use collision shapes among rigit bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
}

void Simulator::addObject (GameObject* o) {
	objList.push_back(o);
	//use default collision group/mask values (dynamic/kinematic/static)
	dynamicsWorld->addRigidBody(o->getBody());
}

void Simulator::stepSimulation(const Ogre::Real elapseTime, int maxSubSteps, const Ogre::Real fixedTimestep){
	//do we need to update positions in simulator for dynamic objects?
	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);
}

#endif // #ifndef __Simulator_h_