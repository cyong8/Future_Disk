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
		/* Fussell originally had a objList as a deque,
		   but I don't know how to work with that so I
		   changed it to a vector
		*/
		//std::deque<GameObject*> objList;  
		int current_object_index; //needs to be setup
		vector<GameObject*> objList;


	public:
		Simulator();
		~Simulator();
		void addObject(GameObject* o);
		btCollisionShape *box_shape;

		// not sure where this removeObject is defined;
		// will most likely have to change it tho
		bool removeObject(GameObject* o);
		void stepSimulation(const Ogre::Real elapsedTime, 	int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
		void setHitFlags(void);
};

#endif // #ifndef __Simulator_h_