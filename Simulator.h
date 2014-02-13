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
	boolremoveObject(GameObject* o);
	void stepSimulation(const Ogre::Real elapsedTime,
	int maxSubSteps = 1, const Ogre::Real ﬁxedTimestep = 1.0f/60.0f);
};