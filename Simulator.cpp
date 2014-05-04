#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Disk.h"
#include "Tile.h"

Simulator::Simulator(Ogre::SceneManager* mSceneMgr, Music* music) 
{
	diskSpeedFactor = 22.0f;
	score = 0;
	soundedJump = false;
	throwFlag = false;

	gameState = NOTSTARTED;
	gameStart = false;

    wallHitAfterThrow = true;
    playerLastThrew = "";
	previousWallHit = "NULL";
	gameDisk = NULL;
	diskSet = false;

	playerList = vector<Player*>(MAX_NUMBER_OF_PLAYERS, NULL);

	for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		PlayerTileIdentity* ptID = new PlayerTileIdentity;
		playerTileIdentities.push_back(ptID);
	}

	gameMusic = music;
	sceneMgr = mSceneMgr;
	// initialize random number generate
    srand(time(0));

	//collision configuration contains default setup for memory, collision setup.
	collisionConfiguration = new btDefaultCollisionConfiguration();
	//use the default collision dispatcher. For parallel processing you can use a different dispatcher
	overlappingPairCache = new btDbvtBroadphase();
	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0.0f,-9.8f, 0.0f));
	dynamicsWorld->debugDrawWorld();
	//keep track of the shapes, we release memory at exit
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;
}
//-------------------------------------------------------------------------------------
Simulator::~Simulator()
{
	gameMusic = NULL;
	delete(collisionConfiguration);
	delete(overlappingPairCache);
	delete(solver);
	delete(dynamicsWorld);
}
//-------------------------------------------------------------------------------------
void Simulator::addObject (GameObject* o) 
{
	o->getBody()->setActivationState(DISABLE_DEACTIVATION);	
	objList.push_back(o); // Add the object to the list of object

	dynamicsWorld->addRigidBody(o->getBody());

	// Set custom btRigidBody WRT specific GameObjects 
	if(o->typeName == "Player")
	{
		playerList[((Player*)o)->getPlayerID() - 1] = (Player*)o; // P1 = playerList[0], P2 = playerList[1]...
		// printf("adding player at index: %d\n\n\n\n", ((Player*)o)->getPlayerID() - 1);
		o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	}
	if(o->typeName == "Disk")
	{
		Player* iPlayer = playerList[((Disk*)o)->checkIDOfHolder() - 1];

		gameDisk = (Disk*)o;
		// printf("\n\n\n adding game disk\n\n\n");
		gameDisk->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setRestitution(1.0f);
		//gameDisk->getSceneNode()->roll(90);
		Ogre::Vector3 toPlayerDirection = iPlayer->getSceneNode()->getPosition().normalisedCopy();

		o->getBody()->setLinearVelocity(btVector3(toPlayerDirection.x, toPlayerDirection.y, toPlayerDirection.z) * btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor));
		gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());
	}
	if(o->typeName == "Target")
	{
		if (o->checkReAddFlag())
		{
			((Target*)o)->toggleHitFlag();
			if (o->getGameObjectName() != "Power" || o->getGameObjectName() != "Speed" || o->getGameObjectName() != "Jump" || o->getGameObjectName() != "Restore") {
			    score = 10;
		    }
		}
		targetList.push_back((Target*)o);
	}
	if(o->typeName == "Wall" || o->typeName == "Tile")
	{
		o->getBody()->setRestitution(0.8f);
	}
	if(o->typeName == "Tile")
	{
	    if (!(o->checkReAddFlag())) 
	    	playerTileIdentities[((Tile*)o)->getTileOwner() - 1]->tileList.push_back((Tile*)o);
        else 
            o->getSceneNode()->setVisible(true);
	}
}
//-------------------------------------------------------------------------------------
GameObject* Simulator::getGameObject(Ogre::String name)
{
	for (int i = 0; i < objList.size(); i++)
	{
		if (Ogre::StringUtil::match(objList[i]->getGameObjectName(), name, true))
		{
			return objList[i];
		}
	}
	return NULL;
}
//-------------------------------------------------------------------------------------
void Simulator::removeObject(Ogre::String name)
{
	if (getGameObject(name)->typeName == "Target")
	{
		for (int i = 0; i < targetList.size(); i++)
		{	
			if (Ogre::StringUtil::match(targetList[i]->getGameObjectName(), name, true))
			{
				targetList.erase(targetList.begin());	
				return;
			}
		}
	}	
	for (int i = 0; i < objList.size(); i++)
	{
		if (Ogre::StringUtil::match(objList[i]->getGameObjectName(), name, true))
		{		
			dynamicsWorld->removeRigidBody(getGameObject(name)->getBody());
			getGameObject(name)->removeFromSimulator();
			objList.erase(objList.begin() + i);
			return;
		}
	}
}
//-------------------------------------------------------------------------------------
void Simulator::stepSimulation(const Ogre::Real elapseTime, int maxSubSteps, const Ogre::Real fixedTimestep)
{
	bool diskHeld = false;

	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);

	for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		Player* localPlayer = playerList[i];

		if (localPlayer != NULL)
		{
			if (localPlayer->checkHolding())
			{
				performThrow(localPlayer);
				diskHeld = true;
			}
			else
			{ 
				if (wallHitAfterThrow) 	
				{
					if (!playerList[i]->checkPlayerCanCatch())
						playerList[i]->togglePlayerCanCatch();
				}
			}
			if (localPlayer->jumpPowerActive)
			{
				localPlayer->jumpTimer--;
				if (localPlayer->jumpTimer <= 0)
					localPlayer->decreaseJump();
			}
		}
	}
	if (!diskHeld)	// Speed disk back up in order to mimic inelasticity
	{	
		if (gameDisk != NULL)
		{
	        btVector3 currentDirection = gameDisk->getBody()->getLinearVelocity().normalized();
            if (gameDisk->powerUp == "Speed")
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor*2.0f, diskSpeedFactor*2.0f, diskSpeedFactor*2.0f));
            else    
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor));

			// if (gameDisk->needsOrientationUpdate)
			// 	adjustDiskOrientation(gameDisk, gameDisk->getBody()->getLinearVelocity(), previousWallHit);

			if(gameDisk->getSceneNode()->getPosition().y < gameRoom->getFloorPositionY())
			{
				((Player*)getGameObject(gameDisk->getPlayerLastThrew()->getGameObjectName()))->attachDisk(gameDisk);
			}
		}
	}
}
//-------------------------------------------------------------------------------------
void Simulator::parseCollisions(void)
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	int i;
	int localGameStart = false;

	for (i=0;i<numManifolds;i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btRigidBody* obA = static_cast<btRigidBody*>(contactManifold->getBody0());
		btRigidBody* obB = static_cast<btRigidBody*>(contactManifold->getBody1());

		OgreMotionState* OMSA = static_cast<OgreMotionState*>(obA->getMotionState());
		OgreMotionState* OMSB = static_cast<OgreMotionState*>(obB->getMotionState());

		GameObject* gA = OMSA->getGameObject();
		GameObject* gB = OMSB->getGameObject();

		if (gA->typeName == "Disk") // If the first object is a disk
			handleDiskCollisions((Disk*)gA, gB);
		else if (gB->typeName == "Disk") // If the second object is a disk
			handleDiskCollisions((Disk*)gB, gA);
		else if ((gA->typeName == "Player" && gB->typeName == "Tile") || (gB->typeName == "Player" && gA->typeName == "Tile"))
		{
			Player* localP;

			if (gA->typeName == "Player")
				localP = (Player*)gA;
			else 
				localP = (Player*)gB;

			if (localP->getBody()->getLinearVelocity().getY() < 5.0f)
				localP->setState(JUMP, false);

			localGameStart = true;
		}
		contactManifold->clearManifold();
	}
	if (localGameStart && gameDisk == NULL)
		diskSet = true;
	// if (soundedJump && groundCheck1)	// played jumping sound, now check if he has hit the ground(landed)
	// {
	// 	//gameMusic->playCollisionSound("Player", "Ground"); // Not sure if I like this collision sound or if it's necessary
	// 	soundedJump = false;
	// }
}
//-------------------------------------------------------------------------------------
void Simulator::setThrowFlag()
{
	throwFlag = !throwFlag;
}
//-------------------------------------------------------------------------------------
void Simulator::performThrow(Player* p)
{
  	btQuaternion diskOrientation;
 	btTransform transform;

	if (throwFlag)
    {	
    	wallHitAfterThrow = false;
        //resetPowerUps();
    	Ogre::Vector3 toParentPosition = gameDisk->getSceneNode()->_getDerivedPosition();

		/* Set the disk direction vector to be the same as the player's sight node vector */
		Ogre::Vector3 diskDirection = p->getPlayerSightNode()->getPosition().normalisedCopy();

		/* The new disk direction is along player's orientation */
		diskDirection = p->getSceneNode()->getOrientation() * diskDirection;

		gameDisk->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setRestitution(1.0f);
		gameDisk->getBody()->setLinearVelocity(btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor) * btVector3(diskDirection.x*1.3f, diskDirection.y*1.3f, diskDirection.z*1.3f));

		gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());

		p->getSceneNode()->removeChild(gameDisk->getSceneNode()); // detach disk from parent
		sceneMgr->getRootSceneNode()->addChild(gameDisk->getSceneNode()); // attach disk to world (root)
		gameDisk->getSceneNode()->setPosition(toParentPosition); // retain the same global position

		throwFlag = false;

		p->togglePlayerCanCatch();

		gameDisk->setPlayerLastThrew(p);
    	p->setHolding(false);
    }
    else // Update position relative to the Player
    {
    	Ogre::Vector3 dpos;
    	float newDiskZ = p->getPlayerDimensions().z;

    	dpos = p->getSceneNode()->getOrientation() * Ogre::Vector3(0.0f, 0.0f, newDiskZ);
		gameDisk->getSceneNode()->_setDerivedPosition(dpos + p->getSceneNode()->getPosition());

		Ogre::Vector3 dpos_derived = gameDisk->getSceneNode()->_getDerivedPosition();
		diskOrientation = btQuaternion(0, 0, 0);
		transform = btTransform(diskOrientation, btVector3(dpos_derived.x, dpos_derived.y, dpos_derived.z)); // might want to do 0.0f in x and y
		gameDisk->getBody()->setCenterOfMassTransform(transform);
    }
}
//-------------------------------------------------------------------------------------
int Simulator::tallyScore(void)
{
	int tmpScore = score;
	score = 0;
	return tmpScore;
}
//-------------------------------------------------------------------------------------
void Simulator::handleDiskCollisions(Disk* disk, GameObject* o)
{
	if (o->typeName == "Wall")
	{	
		wallHitAfterThrow = true;

		if (previousWallHit == "NULL")
		{
			// gameDisk->needsOrientationUpdate = true;
			previousWallHit = o->getGameObjectName();
			// adjustDiskOrientation(disk, disk->getBody()->getLinearVelocity(), previousWallHit);
			gameMusic->playCollisionSound("Disk", "Wall");
		}
		else if (previousWallHit != o->getGameObjectName())
		{
			// disk->needsOrientationUpdate = true;
			previousWallHit = o->getGameObjectName();
			// adjustDiskOrientation(disk, disk->getBody()->getLinearVelocity(), previousWallHit);
			gameMusic->playCollisionSound("Disk", "Wall");
		}
	}
	// Player
	else if (o->typeName == "Player")
	{
		if (((Player*)o)->checkHolding() == false)
		{
			if (((Player*)o)->checkPlayerCanCatch())
			{
				((Player*)o)->attachDisk((Disk*)disk);
				disk->setHolder(((Player*)o)->getPlayerID());
				gameMusic->playCollisionSound("Disk", "Player");
			}
			gameStart = true;
			gameState = STARTED;
			gameMusic->playCollisionSound("Disk", "Player");
		}
	}
	// Target
	else if (o->typeName == "Target")
	{
		if (((Target*)o)->checkHitFlag() == false)
		{
			//printf("COLLIDED WITH TILE!\n\n\n");
		    ((Target*)o)->toggleHitFlag();
			removeObject(o->getGameObjectName());
			if (o->getGameObjectName() == "Power" || o->getGameObjectName() == "Speed" || o->getGameObjectName() == "Jump" || o->getGameObjectName() == "Restore") 
			{
			    o->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
								    ,getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
							       Ogre::Math::RangeRandom(getGameObject("client111")->getSceneNode()->getPosition().y + (2.0f/3.0f)
								    ,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
							       Ogre::Math::RangeRandom(-5.0f, 5.0f));
			    if (disk->activatePowerUp(o->getGameObjectName(), (Player*)getGameObject(disk->getPlayerLastThrew()->getGameObjectName())))
			        // restoreTile();
                gameMusic->playCollisionSound("Disk", "Target");
                if (disk->powerUp == "Speed")
                    gameMusic->playMusic("SpeedUp");
			}
			else
			{		  
				// This is hardcoded right now  
				Ogre::Real width, height, gap;
				width = 30;
				height = 60;
				gap = 10;
				Ogre::Vector3 *bounds = new Ogre::Vector3(width, height, gap);
			    Ogre::Real posx, posy, posz;
				posx = Ogre::Math::RangeRandom(-bounds->x/2.0f, bounds->x/2.0f); // From left to right
				posy = Ogre::Math::RangeRandom(gameRoom->getFloorPositionY()/2.0f, -gameRoom->getFloorPositionY()/2.0f); // From base to top
				posz = Ogre::Math::RangeRandom(0, -bounds->y/2.0f + 10.0f); // From gap front to room back
			    o->getSceneNode()->setPosition(0, 0, 0);
			    gameMusic->playCollisionSound("Disk", "Target");
			    cout << o->getGameObjectName() << "'s new position position is " << "(" << posx << ", " << posy << ", " << posz << ")\n";
		    }
			o->addToSimulator();
		}
	}
	else if (o->typeName == "Tile" && !((Tile *)o)->checkHitFlag() && wallHitAfterThrow && !playerList[((Disk*)disk)->checkIDOfHolder() - 1]->checkHolding() && playerList[1] != NULL)  // HARD CODE PLAYER FLAG
	{	
		((Tile *)o)->toggleHitFlag(); // Mark that the tile has been hit

		destroyTiles((Tile*)o);

		disk->resetPowerUp();
		((Player*)getGameObject(disk->getPlayerLastThrew()->getGameObjectName()))->attachDisk((Disk*)disk);

		newRemovedTile = true;
	}
}

//-------------------------------------------------------------------------------------
void Simulator::adjustDiskOrientation(Disk* d, btVector3 currVelocity, Ogre::String wallName)
{
	int changePitch = 0;
	int changeRoll = 0;
	btTransform trans = d->getBody()->getCenterOfMassTransform();
	btQuaternion quat;

	if (Ogre::StringUtil::match(wallName, "FarLeftWall", true) || Ogre::StringUtil::match(wallName, "FarRightWall", true) 
				|| Ogre::StringUtil::match(wallName, "NearRightWall", true) || Ogre::StringUtil::match(wallName, "NearLeftWall", true))	
		quat = btQuaternion(0.0f, -d->getSceneNode()->getOrientation().getPitch().valueRadians(), -d->getSceneNode()->getOrientation().getRoll().valueRadians());
    if (Ogre::StringUtil::match(wallName, "LeftWall", true) || Ogre::StringUtil::match(wallName, "RightWall", true))	
    	quat = btQuaternion(0.0f, 0.0f, -d->getSceneNode()->getOrientation().getRoll().valueRadians());
    if (Ogre::StringUtil::match(wallName, "FarWall", true) || Ogre::StringUtil::match(wallName, "NearWall", true))
    	quat = btQuaternion(0.0f, -d->getSceneNode()->getOrientation().getPitch().valueRadians(), 0.0f);
    if (Ogre::StringUtil::match(wallName, "Ceiling", true))
    	quat = btQuaternion(0.0f, 0.0f, -d->getSceneNode()->getOrientation().getRoll().valueRadians());

    trans.setRotation(quat);

	d->getBody()->setCenterOfMassTransform(trans);
	d->needsOrientationUpdate = false;
}
//-------------------------------------------------------------------------------------
void Simulator::handlePlayerCollisions(GameObject* cPlayer, GameObject* o)
{
}
//-------------------------------------------------------------------------------------
bool Simulator::checkGameStart()
{
	return gameStart;
}
//---------------------------------------------------------------------------------------
void Simulator::restoreTile() // TILE UPDATE FLAG
{ 
	// if (playerTileIdenttites)

    // if (playerList[0] != NULL && playerList[0]->getGameObjectName() == gameDisk->getPlayerLastThrew()->getGameObjectName() && hostRemoveIndexes.size() > 0) {
    //     hostTileList[hostRemoveIndexes.back()]->addToSimulator();
    //     hostRemoveIndexes.pop_back();
    //     gameMusic->playMusic("Restore");
    // }
    // else if (playerList[1] != NULL && playerList[0]->getGameObjectName() == gameDisk->getPlayerLastThrew()->getGameObjectName() && clientRemoveIndexes.size() > 0) {
    //     clientTileList[clientRemoveIndexes.back()]->addToSimulator();
    //     clientRemoveIndexes.pop_back();
    //     gameMusic->playMusic("Restore");
    // }
}
//-------------------------------------------------------------------------------------
void Simulator::destroyTiles(Tile* t)  // TILE UPDATE FLAG
{
	int playerID = t->getTileOwner() - 1;
	int tileIndex = t->getTileNumber();

    removeObject(t->getGameObjectName());
    // printf("*******Destorying Tiles....");
    // printf("\tPlayer ID = %d\tTile Index = %d\n\n\n", playerID + 1, tileIndex);
    playerTileIdentities[playerID]->removedTiles.push_back(t);
    playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex);


    /* When removing from client - add to clientRemoveIndexes */
    if (gameDisk->powerUp == "Power")
    {
        int tPerRow = tPerRow;
        int tPerCol = gameRoom->getTilesPerColumn();

        int col = tileIndex % tPerRow;

        if (col != 0) // remove left - not in far left column
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex]->checkHitFlag()))
            {
                playerTileIdentities[playerID]->tileList[tileIndex - 1]->toggleHitFlag();

                playerTileIdentities[playerID]->removedTiles.push_back(playerTileIdentities[playerID]->tileList[tileIndex - 1]);
    			playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex - 1);

                removeObject(playerTileIdentities[playerID]->tileList[tileIndex - 1]->getGameObjectName());
            }
        }
        if (col != (tPerRow - 1)) // remove right - not in far right column
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex + 1]->checkHitFlag()))
            {
                playerTileIdentities[playerID]->tileList[tileIndex + 1]->toggleHitFlag();

                playerTileIdentities[playerID]->removedTiles.push_back(playerTileIdentities[playerID]->tileList[tileIndex + 1]);
    			playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex + 1);

                removeObject(playerTileIdentities[playerID]->tileList[tileIndex + 1]->getGameObjectName());
            }
        }
        if (tileIndex > tPerRow) // remove top - not in top row
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->checkHitFlag()))
            {
                playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->toggleHitFlag();

                playerTileIdentities[playerID]->removedTiles.push_back(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]);
    			playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex - tPerRow);

                removeObject(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->getGameObjectName());
            }
        }
        if (tileIndex < (tPerRow * (tPerCol - 1))) // remove bottom - not in bottom row
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex + tPerRow]->checkHitFlag()))
            {
                playerTileIdentities[playerID]->tileList[tileIndex + tPerRow]->toggleHitFlag();

				playerTileIdentities[playerID]->removedTiles.push_back(playerTileIdentities[playerID]->tileList[tileIndex + tPerRow]);
    			playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex + tPerRow);

                removeObject(playerTileIdentities[playerID]->tileList[tileIndex + tPerRow]->getGameObjectName());
            }
        }
        gameMusic->playMusic("BigBlast");
    }
    else
        gameMusic->playMusic("Blast");
}
//-------------------------------------------------------------------------------------
void Simulator::resetSimulator()
{
    for (int i = 0; i < playerList.size(); i++) 
    {
        if (playerList[i] != NULL)
        {	    
            if (playerList[i]->checkHolding()) 
            {
                setThrowFlag();
                performThrow(playerList[i]);
            }
            
            removeObject(playerList[i]->getGameObjectName());
            playerList[i]->getSceneNode()->_setDerivedPosition(playerList[i]->getStartingPosition());
        }
    }
    
    removeObject(gameDisk->getGameObjectName()); 
    gameDisk->getSceneNode()->_setDerivedPosition(Ogre::Vector3(0, 0, 0));   

    soundedJump = false;
	throwFlag = false;

	gameState = NOTSTARTED;
	gameStart = false;

    wallHitAfterThrow = true;
    playerLastThrew = "";
	previousWallHit = "NULL";
	gameDisk = NULL;
	diskSet = false;
}
//-------------------------------------------------------------------------------------
void Simulator::removePlayer(int playerIndex)
{
	removeObject(playerList[playerIndex]->getGameObjectName());
	playerList[playerIndex] = NULL;
}
//-------------------------------------------------------------------------------------
void Simulator::setGameRoom(Room* rm)
{
	gameRoom = rm;
}
//-------------------------------------------------------------------------------------
void Simulator::removeTiles(vector<Tile*>& rt)
{
	for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		if (playerList[i] != NULL && playerList[i]->checkState(PLAYING))
		{
			PlayerTileIdentity* localIdentity = playerTileIdentities[i];
			for (int j = 0; j < localIdentity->removedTileIndices.size(); j++)
			{
				Tile* tileRemoved = localIdentity->tileList[localIdentity->removedTileIndices[j]];
				rt.push_back(tileRemoved);
				// printf("NEW REMOVED TILE - Player: %d\t Tile #: %d\n\n", tileRemoved->getTileOwner(), tileRemoved->getTileNumber());
			}
			localIdentity->removedTileIndices.clear();
		}
	}
}
//-------------------------------------------------------------------------------------
