#ifndef __Simulator_h_
#define __Simulator_h_

#include "BaseApplication.h"

class GameObject;

class Simulator
{
	protected:
		btDefaultCollisionConfiguration* collisionConfiguration;
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
		void stepSimulation(const Ogre::Real elapsedTime, 	int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
};

#endif // #ifndef __Simulator_h_