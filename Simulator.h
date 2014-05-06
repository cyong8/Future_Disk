#ifndef __Simulator_h_
#define __Simulator_h_

#include "BaseApplication.h"
#include "Target.h"
#include "Music.h"

using namespace std;

class GameObject;
class Player;
class PlayerCamera;
class Disk;
class Music;
class Room;
class Tile;

enum gameState{NOTSTARTED, STARTED, PAUSED, OVER};

struct PlayerTileIdentity
{
	vector<Tile*> tileList; 
	vector<Tile*> removedTiles;
	vector<int> removedTileIndices;
};

class Simulator
{
	protected:
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btBroadphaseInterface* overlappingPairCache;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamicsWorld;
		btConstraintSolver* mConstraintsolver;

		Ogre::SceneManager* sceneMgr;
		
		vector<GameObject*> objList;
		vector<Target*> targetList;
		vector<Player*> playerList;
		vector<Target*> removedPowerUps;
		Disk* gameDisk;
		Room* gameRoom;
	
			/* Tile Objects */
		vector<PlayerTileIdentity*> playerTileIdentities;
		bool newRemovedTile;
	
		Ogre::String playerLastThrew;
		int score;
		bool throwFlag;
		bool wallHitAfterThrow;
		
		bool gameStart;
		bool diskSet;
		enum gameState gameState;	// Not doing anything right now
		
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
		void handleDiskCollisions(Disk* disk, GameObject* o);
		void adjustDiskOrientation(Disk *d, btVector3 currVelocity, Ogre::String wallName);
		void handlePlayerCollisions(GameObject* cPlayer, GameObject* o);
		bool checkGameStart(void);
		void restoreTile(void);
		void destroyTiles(Tile* t);
		void setFloorY(Ogre::Real);
		void resetSimulator(void);
		void removePlayer(int playerIndex);
		void setGameRoom(Room* rm);
		void removeTiles(vector<Tile*>& rt);
		void removeHitPowerUps(vector<Target*>& pt);
		bool checkDiskSet() { return diskSet; };

		bool soundedJump;
};

#endif // #ifndef __Simulator_h_
