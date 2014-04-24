#ifndef __Simulator_h_
#define __Simulator_h_

#include "BaseApplication.h"
#include <vector>
#include <iostream>
#include "Target.h"
#include "Music.h"

using namespace std;

class GameObject;
class Player;
class PlayerCamera;
class Disk;
class Music;

enum gameState{NOTSTARTED, STARTED, PAUSED, OVER};

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
		vector<Player*> playerList;
		
		PlayerCamera* player1Cam;
		PlayerCamera* player2Cam;
		Ogre::String playerLastThrew;
		int score;
		bool viewChangeP1;
		bool viewChangeP2;
		bool throwFlag;
		bool wallHitAfterThrow;
		
		bool gameStart;
		enum gameState gameState;

		bool player1CanCatch;
	    bool player2CanCatch;
		bool giveDisk;
		
		Ogre::String previousWallHit;
		Ogre::Real diskSpeedFactor;
		
		Music* gameMusic;

	public:
		Simulator(Ogre::SceneManager* mSceneMgr, Music* music);
		~Simulator();
		void addObject(GameObject* o);
		void removeObject(Ogre::String name);
		void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
		void parseCollisions(void);
		void setCamera(PlayerCamera* pcam);
		GameObject* getGameObject(Ogre::String name);
		void setThrowFlag(void);
		void performThrow(Player* p);
		int tallyScore(void);
		bool checkOnFloor(void);
		void resetOnFloor(void);
		void handleDiskCollisions(GameObject* disk, GameObject* o);
		void adjustDiskOrientation(Disk *d, btVector3 currVelocity, Ogre::String wallName);
		void handlePlayerCollisions(GameObject* cPlayer, GameObject* o);
		bool checkGameStart(void);
		void restoreTile(void);
		void destroyTiles(vector<GameObject*>& tileList, vector<int>& removeIndexes, int index);
		void resetSimulator();
		GameObject* objListCheck(int index);
		bool soundedJump;
		Disk* gameDisk;
		bool setDisk;
		vector<GameObject*> hostTileList;
		vector<GameObject*> clientTileList;
		vector<int> hostRemoveIndexes;
		vector<int> clientRemoveIndexes;
		bool newRemovedTile;
};

#endif // #ifndef __Simulator_h_
