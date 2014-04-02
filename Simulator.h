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

enum powerUp{NONE, POWER, SPEED, SHIELD, JUMP, RESTORE};

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

		Player* p1;
		Player* p2;
		PlayerCamera* player1Cam;
		PlayerCamera* player2Cam;
		Ogre::String playerLastThrew;
		int score;
		bool viewChangeP1;
		bool viewChangeP2;
		bool throwFlag;
		bool gameStart;
		bool player1CanCatch;
	    bool player2CanCatch;
		bool giveDisk;
	   	bool wallHitAfterThrow;
		Ogre::String previousWallHit;
		Ogre::Real diskSpeedFactor;
		powerUp currentPower;
		
		Music* gameMusic;

	public:
		Simulator(Ogre::SceneManager* mSceneMgr, Music* music);
		~Simulator();
		void addObject(GameObject* o);
		void removeObject(Ogre::String name);
		void stepSimulation(const Ogre::Real elapsedTime, int maxSubSteps = 1, const Ogre::Real fixedTimestep = 1.0f/60.0f);
		void parseCollisions(void);
		void setCamera(PlayerCamera* pcam);
		void setPlayer(Player* p);
		GameObject* getGameObject(Ogre::String name);
		PlayerCamera* getPlayerCamera(Ogre::String name);
		void toggleViewChange(Ogre::String name);
		void setThrowFlag(void);
		void performThrow(Player* p);
		int tallyScore(void);
		bool checkOnFloor(void);
		void resetOnFloor(void);
		void handleDiskCollisions(GameObject* disk, GameObject* o);
		void adjustDiskOrientation(Disk *d, btVector3 currVelocity, Ogre::String wallName);
		void handlePlayerCollisions(GameObject* cPlayer, GameObject* o);
		void updatePlayerCamera(PlayerCamera* cam, const Ogre::Real elapseTime);
		bool checkGameStart(void);
		void restoreTile(void);
		void destroyTiles(vector<GameObject*>& tileList, vector<int>& removeIndexes, int index);
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
