class Simulator {
	protected:
		btDefaultCollisionConﬁguration* collisionConﬁguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btConstraintSolver* mConstraintsolver;
	//  btCollisionWorld* mWorld;
		Ogre::SceneManager* sceneMgr;
		std::deque<GameObject*> objList;

	public:
		Simulator();
		~Simulator();
		void addObject(GameObject* o);
		bool removeObject(GameObject* o);
		void stepSimulation(const Ogre::Real elapsedTime,
				int maxSubSteps = 1, const Ogre::Real ﬁxedTimestep = 1.0f/60.0f);
};

Simulator::Simulator() {
 	///collision conﬁguration contains default setup for memory, collision setup.
	collisionConﬁguration = new btDefaultCollisionConﬁguration();
 	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher
	 dispatcher = new btCollisionDispatcher(collisionConﬁguration);
	 ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();
	 ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	 solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConﬁguration);
	dynamicsWorld->setGravity(btVector3(0,-0.098, 0));
	 //keep track of the shapes, we release memory at exit.
	 //make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
}

void Simulator::addObject (GameObject* o) {
	objList.push_back(o);
	// use default collision group/mask values (dynamic/kinematic/static)
	dynamicsWorld->addRigidBody(o->getBody()); 
}

void Simulator::stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps, const Ogre::Real ﬁxedTimestep) {
	// do we need to update positions in simulator for dynamic objects?
	dynamicsWorld->stepSimulation(elapsedTime, maxSubSteps, ﬁxedTimestep);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        BasicTut2 app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif