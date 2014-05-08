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
	safeToSpawnPowerUps = false;

	playerList = vector<Player*>(MAX_NUMBER_OF_PLAYERS, NULL);

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
		printf("\n\n\nADDING DISK TO SIMULATOR!!! Initial holder = %d\n\n\n", ((Disk*)o)->checkIDOfHolder());
		gameDisk = (Disk*)o;
		// printf("\n\n\n adding game disk\n\n\n");
		gameDisk->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setRestitution(1.0f);
		//gameDisk->getSceneNode()->roll(90);
		if(!o->checkReAddFlag())
		{
			Ogre::Vector3 toPlayerDirection = iPlayer->getSceneNode()->getPosition().normalisedCopy();
			o->getBody()->setLinearVelocity(btVector3(toPlayerDirection.x, toPlayerDirection.y, toPlayerDirection.z) * btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor));
			gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());
		}

	}
	if(o->typeName == "Target")
	{
		o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		o->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));

		if (o->checkReAddFlag())
			((Target*)o)->toggleHitFlag();
		else
		{
			if (((Target*)o)->getPowerUpType() == TARGET) 
				targetList.push_back((Target*)o);
		}
	}
	if(o->typeName == "Wall")
	{
		o->getBody()->setRestitution(0.8f);
	}
	if(o->typeName == "Tile")
	{
		o->getBody()->setRestitution(0.8f);

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
	for (int i = 0; i < objList.size(); i++)
	{
		Ogre::String localName = objList[i]->getGameObjectName();
		if (Ogre::StringUtil::match(localName, name, true))
		{		
			if (objList[i]->typeName == "Target" && ((Target*)objList[i])->getPowerUpType() == TARGET)
			    score = 10;

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
            if (gameDisk->checkActivePowerUp() == SPEED)
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor*2.0f, diskSpeedFactor*2.0f, diskSpeedFactor*2.0f));
            else    
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor));

			// if (gameDisk->needsOrientationUpdate)
			// 	adjustDiskOrientation(gameDisk, gameDisk->getBody()->getLinearVelocity(), previousWallHit);
		}
	}
	if (gameDisk != NULL)
	{
		if(gameDisk->getSceneNode()->getPosition().y < gameRoom->getFloorPositionY() - 5.0f)
		{
			int lowestTileCountPlayer = -1;
			int lowestCount = 0;
			int chooseGiveToNewLower;
			
			for (int i = 0; i < playerTileIdentities.size(); i++)
			{	
				if (playerList[i] != NULL && !playerList[i]->checkHolding() && gameDisk->getPlayerLastThrew()->getPlayerID() != i + 1 &&
					playerTileIdentities[i]->removedTiles.size() >= lowestCount)
				{
					if (lowestTileCountPlayer != -1)
					{
						chooseGiveToNewLower = rand() % 2 + 1;

						if (chooseGiveToNewLower == 1)
							lowestTileCountPlayer = i;
					}
					else
						lowestTileCountPlayer = i;
				}
			}
			if (lowestTileCountPlayer == -1)
				playerList[gameDisk->getPlayerLastThrew()->getPlayerID() - 1]->attachDisk(gameDisk);
			else
				playerList[lowestTileCountPlayer]->attachDisk(gameDisk);
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
    	Ogre::Vector3 toFrontOfPlayer =  p->getSceneNode()->getOrientation() * (Ogre::Vector3(0.0f, 0.0f, p->getPlayerDimensions().z));
		gameDisk->getSceneNode()->setPosition(toFrontOfPlayer + p->getSceneNode()->getPosition()); // retain the same global position

    	gameDisk->addToSimulator();
    	wallHitAfterThrow = false;
        //resetPowerUps();

		/* Set the disk direction vector to be the same as the player's sight node vector */
		Ogre::Vector3 diskDirection = p->getPlayerSightNode()->getPosition().normalisedCopy();

		/* The new disk direction is along player's orientation */
		diskDirection = p->getSceneNode()->getOrientation() * diskDirection;

		gameDisk->getBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		gameDisk->getBody()->setRestitution(1.0f);
		gameDisk->getBody()->setLinearVelocity(btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor) * btVector3(diskDirection.x*1.3f, diskDirection.y*1.3f, diskDirection.z*1.3f));

		gameDisk->setThrownVelocity(gameDisk->getBody()->getLinearVelocity());

		// p->getSceneNode()->removeChild(gameDisk->getSceneNode()); // detach disk from parent
		// sceneMgr->getRootSceneNode()->addChild(gameDisk->getSceneNode()); // attach disk to world (root)

		throwFlag = false;

		p->togglePlayerCanCatch();

		gameDisk->setPlayerLastThrew(p);
    	p->setHolding(false);
    	predictedHit();
    }
    else // Update position relative to the Player
    {
    	// Ogre::Vector3 dpos;
    	// float newDiskZ = p->getPlayerDimensions().z;

    	// dpos = p->getSceneNode()->getOrientation() * Ogre::Vector3(0.0f, 0.0f, newDiskZ);
		gameDisk->getSceneNode()->_setDerivedPosition(p->getSceneNode()->getPosition());

		// Ogre::Vector3 dpos_derived = gameDisk->getSceneNode()->_getDerivedPosition();
		// diskOrientation = btQuaternion(0, 0, 0);
		// transform = btTransform(diskOrientation, btVector3(dpos_derived.x, dpos_derived.y, dpos_derived.z)); // might want to do 0.0f in x and y
		// gameDisk->getBody()->setCenterOfMassTransform(transform);
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
				safeToSpawnPowerUps = true;
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
		    ((Target*)o)->toggleHitFlag();
			removeObject(o->getGameObjectName());
			if (((Target*)o)->getPowerUpType() != TARGET)
			{
				powerUpType puType = ((Target*)o)->getPowerUpType();		

			    if (disk->activatePowerUp(puType, disk->getPlayerLastThrew()))
			        restoreTile(disk->getPlayerLastThrew()->getPlayerID() - 1);
                
                gameMusic->playCollisionSound("Disk", "Target");

             	if (puType == EXPLOSIVE || puType == SPEED)
             		((Target*)o)->setReceiverID(disk->getDiskID());
             	else
                	((Target*)o)->setReceiverID(disk->getPlayerLastThrew()->getPlayerID());
                
                removedPowerUps.push_back((Target*)o);
			}
			else
			{
				Ogre::Real width, height, gap;
			    Ogre::Real posx, posy, posz;

				width = gameRoom->getWidth();
				height = gameRoom->getHeight();
				gap = gameRoom->getGapSize();

				posx = Ogre::Math::RangeRandom(-width/2.0f, width/2.0f);
				posy = Ogre::Math::RangeRandom(gameRoom->getFloorPositionY()/2.0f, -gameRoom->getFloorPositionY()/2.0f);
				posz = Ogre::Math::RangeRandom(-gap/2.0f, -height/2.0f);
			    o->getSceneNode()->setPosition(posx, posy, posz);
			    gameMusic->playCollisionSound("Disk", "Target");
				o->addToSimulator();
		    }
		}
	}
	else if (o->typeName == "Tile" && !((Tile *)o)->checkHitFlag() && wallHitAfterThrow && !playerList[((Disk*)disk)->getPlayerLastThrew()->getPlayerID() - 1]->checkHolding() && playerList[1] != NULL)  // HARD CODE PLAYER FLAG
	{	
		((Tile *)o)->toggleHitFlag(); // Mark that the tile has been hit

		destroyTiles((Tile*)o);

		disk->resetPowerUp();
		((Player*)getGameObject(disk->getPlayerLastThrew()->getGameObjectName()))->attachDisk((Disk*)disk);

		newRemovedTile = true;
	}
	if(sceneMgr->hasManualObject("Circle"))
	{
		sceneMgr->destroyManualObject("Circle");
		sceneMgr->destroySceneNode("CircleNode");
		predictedHit();
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
void Simulator::restoreTile(int playerID)
{ 	
	int numTilesRemoved = playerTileIdentities[playerID]->removedTiles.size();

	if (playerTileIdentities[playerID]->removedTiles.size() > 0)
	{
		int ranTileToRestore = rand() % numTilesRemoved;
		Tile* resTile = playerTileIdentities[playerID]->removedTiles[ranTileToRestore];

		resTile->addToSimulator();
		playerTileIdentities[playerID]->removedTiles.erase(playerTileIdentities[playerID]->removedTiles.begin() + ranTileToRestore);
        gameMusic->playMusic("Restore");
	}
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
    if (gameDisk->checkActivePowerUp() == EXPLOSIVE)
    {
        int tPerRow = gameRoom->getTilesPerRow();
        int tPerCol = gameRoom->getTilesPerColumn();

        int col = tileIndex % tPerRow;

        printf("Tiles: Per row = %d\t Per col = %d\n\n", tPerRow, tPerCol);

        if (col != 0) // remove left - not in far left column
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex - 1]->checkHitFlag()))
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
        if (tileIndex > tPerRow) // Can remove bottom
        { 
            if (!(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->checkHitFlag()))
            {
                playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->toggleHitFlag();

                playerTileIdentities[playerID]->removedTiles.push_back(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]);
    			playerTileIdentities[playerID]->removedTileIndices.push_back(tileIndex - tPerRow);

                removeObject(playerTileIdentities[playerID]->tileList[tileIndex - tPerRow]->getGameObjectName());
            }
        }
        if (tileIndex < (tPerRow * (tPerCol - 1))) // Can remove top one
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
	for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		PlayerTileIdentity* ptID = new PlayerTileIdentity;
		playerTileIdentities.push_back(ptID);
	}

	gameRoom = rm;
}
//-------------------------------------------------------------------------------------
void Simulator::removeTiles(vector<Tile*>& rt)
{
	for (int i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		if (playerList[i] != NULL && playerList[i]->getPlayerGameState() == PLAYING)
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
void Simulator::removeHitPowerUps(vector<Target*>& pt)
{
	for (int i = 0; i < removedPowerUps.size(); i++)
	{
		pt.push_back(removedPowerUps[i]);
		removedPowerUps[i]->getSceneNode()->setVisible(false);
		// removeObject(removedPowerUps[i]->getGameObjectName());
	}
	removedPowerUps.clear();
}
//-------------------------------------------------------------------------------------
void Simulator::predictedHit()
{
	// Get the velocity vector of the disk
	// btVector3 diskVector = gameDisk->getBody()->getLinearVelocity();
	// Ogre::Vector3 normalOfWall = Ogre::Vector3(0,0,0); // Initialize to 0
	// Ogre::Vector3 pointOnWall = Ogre::Vector3(0,0,0); // Initialize to 0
	// Ogre::Vector3 originalPosition = gameDisk->getSceneNode()->getPosition();
	// Ogre::Vector3 intersectionPoint;
	// bool intersectionFound = false;
	// float t;
	// int i;
	// // Find where it intersects with the next surface
	// while(!intersectionFound)
	// {
	// 	// For each wall - cycle through
	// 	for(i = 0; i < 8; i++)
	// 	{
	// 		// Get wall
	// 		Wall *wall = gameRoom->getWall(i);
	// 		// Normal of the wall
	// 		normalOfWall = wall->getNormal();
	// 		// Point on the wall
	// 		pointOnWall = wall->getCenter();

	// 		t = (normalOfWall.x * (pointOnWall.x - originalPosition.x) + normalOfWall.y * (pointOnWall.y - originalPosition.y) + normalOfWall.z * (pointOnWall.z - originalPosition.z)) / (normalOfWall.x * diskVector.getX() + normalOfWall.y * diskVector.getY() + normalOfWall.z * diskVector.getZ());

	// 		// There is an intersection if t > 0
	// 		if(t > 0)
	// 		{
	// 			// Calculate the position
	// 			intersectionPoint = Ogre::Vector3(originalPosition.x + diskVector.getX() * t, originalPosition.y + diskVector.getY() * t, originalPosition.z + diskVector.getZ() * t);
	// 			// When you find an intersection, break
	// 			intersectionFound = true;
	// 			printf("intersection found at x = %f y = %f z= %f\n", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
	// 			// Draw circle at point
	// 			Ogre::ManualObject* Circle=sceneMgr->createManualObject("Circle");
	// 			Ogre::SceneNode* CircleNode=sceneMgr->getRootSceneNode()->createChildSceneNode("CircleNode");
	// 			Circle->begin("BaseWhite", Ogre::RenderOperation::OT_LINE_STRIP);
				 
	// 			const float accuracy = 30;
	// 			const float radius = 5/100.0f;
	// 			unsigned int index = 0;
	// 			for(float theta = 0; theta <= 2 * Ogre::Math::PI; theta += Ogre::Math::PI / accuracy)
	// 			{
	// 			Circle->position(cos(theta)*radius, 0, sin(theta)*radius);
	// 			Circle->index(index++);
	// 			}
				 
	// 			Circle->end();
	// 			CircleNode->attachObject(Circle);
	// 			break;
	// 		}
	// 	// If we've gotten here then an intersection hasn't been found - which actually means something went wrong.
	// 	if(i == 7)
	// 		printf("We got to 7 and didn't break. Oops. \n\n\n");
	// 	}
	// }
	// Draw a circle on the ground at the point of intersection
}