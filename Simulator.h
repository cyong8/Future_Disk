#ifndef __Simulator_h_
#define __Simulator_h_

#include "BaseApplication.h"

class GameObject;
class Player;
class PlayerCamera;

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
		vector<GameObject*> objList;

		// Attributes to update PlayerCamera
		Player* p1;
		Player* p2;
		PlayerCamera* player1Cam;
		PlayerCamera* player2Cam;
		bool viewChangeP1;
		bool viewChangeP2;
		bool throwFlag;

	public:
		Simulator();
		~Simulator();
		void addObject(GameObject* o);
		btCollisionShape *box_shape;

		void removeObject(Ogre::String name);
		void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
		void setHitFlags(void);
		void setCamera(PlayerCamera* pcam);
		void setPlayer(Player* p);
		GameObject* getGameObject(Ogre::String name);
		PlayerCamera* getPlayerCamera(Ogre::String name);
		void toggleViewChange(Ogre::String name);
		void setThrowFlag(void);
};

#endif // #ifndef __Simulator_h_