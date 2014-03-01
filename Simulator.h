#ifndef __Simulator_h_
#define __Simulator_h_

#include "BaseApplication.h"
#include <vector>
#include "Target.h"
using namespace std;

class GameObject;
class Player;
class PlayerCamera;
class Disk;

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
		vector<Target*> targetList;

		// Attributes to update PlayerCamera
		Player* p1;
		Player* p2;
		PlayerCamera* player1Cam;
		PlayerCamera* player2Cam;
		Disk* gameDisk;
		int score;
		bool viewChangeP1;
		bool viewChangeP2;
		bool throwFlag;
		bool allowMovement;
		bool onFloor;

	public:
		Simulator(Ogre::SceneManager* mSceneMgr);
		~Simulator();
		void addObject(GameObject* o);
		void removeObject(Ogre::String name);
		void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
		void setHitFlags(void);
		void setCamera(PlayerCamera* pcam);
		void setPlayer(Player* p);
		GameObject* getGameObject(Ogre::String name);
		PlayerCamera* getPlayerCamera(Ogre::String name);
		void toggleViewChange(Ogre::String name);
		void setThrowFlag(void);
		int tallyScore(void);
		bool allowMovementCheck(void);
		bool checkOnFloor(void);
		void resetOnFloor(void);
};

#endif // #ifndef __Simulator_h_