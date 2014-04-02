#include "Simulator.h"
#include "GameObject.h"
#include "OgreMotionState.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Disk.h"


Simulator::Simulator(Ogre::SceneManager* mSceneMgr, Music* music) 
{
	p1 = NULL;
	p2 = NULL;
	diskSpeedFactor = 15.0f;

	score = 0;
	soundedJump = false;
	viewChangeP1 = false;
	viewChangeP2 = false;
	throwFlag = false;
	gameStart = false;
	player1CanCatch = true;
    player2CanCatch = true;
    wallHitAfterThrow = true;
    playerLastThrew = "";
	previousWallHit = "NULL";
	gameDisk = NULL;
	setDisk = false;
	currentPower = NONE;

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
    static int tileNumber = 0;
	o->getBody()->setActivationState(DISABLE_DEACTIVATION);	
	objList.push_back(o); // Add the object to the list of object

	dynamicsWorld->addRigidBody(o->getBody());
	// Set custom btRigidBody WRT specific GameObjects 
	if(o->typeName == "Player")
	{
		if (o->getGameObjectName() == "Player1")
		{
			setPlayer((Player*)o);
			player1Cam->initializePosition(((Player*)o)->getPlayerCameraNode()->_getDerivedPosition(), ((Player*)o)->getPlayerSightNode()->_getDerivedPosition());
			player1Cam->setPlayer((Player*)o);
		}
		if (o->getGameObjectName() == "Player2")
			setPlayer((Player*)o);

		o->getBody()->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	}
	if(o->typeName == "Disk")
	{
		Player* iPlayer = (Player*)getGameObject(((Disk*)o)->checkInitialPlayer());

		gameDisk = (Disk*)o;
		printf("\n\n\n adding game disk\n\n\n");
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
	    if (!(o->checkReAddFlag())) {
	        if (tileNumber < 42) {
	            clientTileList.push_back(o);
	            ((Tile*)clientTileList[tileNumber])->indexIntoTileArray = tileNumber;
            }
            else {
                hostTileList.push_back(o);
                ((Tile*)hostTileList[tileNumber-42])->indexIntoTileArray = tileNumber-42;
            }
        }
        else {
            o->getSceneNode()->setVisible(true);
        }
        tileNumber++;
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
		}
	}
}
//-------------------------------------------------------------------------------------
void Simulator::stepSimulation(const Ogre::Real elapseTime, int maxSubSteps, const Ogre::Real fixedTimestep)
{
	Ogre::Vector3 sightPosBeforeSim1;
	Ogre::Vector3 sightPosBeforeSim2;
	if (p1)
		// keep track of sight node position to reapply it after simulator is stepped 
		sightPosBeforeSim1 = p1->getPlayerSightNode()->getPosition(); 

	dynamicsWorld->stepSimulation(elapseTime, maxSubSteps, fixedTimestep);

	if (p1)
		p1->getPlayerSightNode()->setPosition(sightPosBeforeSim1);
	// update the rotation of the disk scene node
	//gameDisk->rotateOffWall();
	if (player1Cam)
		updatePlayerCamera(player1Cam, elapseTime);

	if (p1->checkHolding() || (p2 != NULL))
    {
    	if(p2 != NULL)
    	{
	    	if (p2->checkHolding())
	    	{
	    		performThrow(p2);
	    	}
    	}
    	if (p1->checkHolding())
    		performThrow(p1);
    }
	else	// Speed disk back up in order to mimic inelasticity
	{	
		if (gameDisk != NULL)
		{
	        btVector3 currentDirection = gameDisk->getBody()->getLinearVelocity().normalized();
            if (gameDisk->powerUp == "Speed")
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor*2.0f, diskSpeedFactor*2.0f, diskSpeedFactor*2.0f));
            else    
                gameDisk->getBody()->setLinearVelocity(currentDirection * btVector3(diskSpeedFactor, diskSpeedFactor, diskSpeedFactor));

			if (gameDisk->needsOrientationUpdate)
				adjustDiskOrientation(gameDisk, gameDisk->getBody()->getLinearVelocity(), previousWallHit);

			if(gameDisk->getSceneNode()->getPosition().y < -30.0f)
			{
				if (p2 != NULL)
					;

				((Player*)getGameObject(playerLastThrew))->attachDisk(gameDisk);
			}
		}
	}
    if (p1 != NULL && p1->jumpPowerActive) {
        p1->jumpTimer--;
        if (p1->jumpTimer <= 0)
            p1->decreaseJump();
    }
    if (p2 != NULL && p2->jumpPowerActive) {
        p2->jumpTimer--;
        if (p2->jumpTimer <= 0)
            p2->decreaseJump();
    }
}
//-------------------------------------------------------------------------------------
void Simulator::parseCollisions(void)
{
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	int i;
	int groundCheck1 = false; //checking Tile taking care of multiple collisions
	int groundCheck2 = false;
	Player* colP;

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
			handleDiskCollisions(gA, gB);
		else if (gB->typeName == "Disk") // If the second object is a disk
			handleDiskCollisions(gB, gA);
		else if ((gA->typeName == "Player" && gB->typeName == "Tile") || (gB->typeName == "Player" && gA->typeName == "Tile"))
		{
			if (!groundCheck1 && ((gA->getGameObjectName() == "Player1") || (gB->getGameObjectName() == "Player1")))
				groundCheck1 = true;
			if (!groundCheck2 && ((gA->getGameObjectName() == "Player2") || (gB->getGameObjectName() == "Player2")))
				groundCheck2 = true;
		}
		contactManifold->clearManifold();
	}
	if (!groundCheck1)
		soundedJump = true;
	else
		p1->groundConstantSet = false;

	if (!groundCheck2)
		soundedJump = true;
	else
		p2->groundConstantSet = false;

	if ((groundCheck1 || groundCheck2) && gameDisk == NULL)
    	setDisk = true;
	if (soundedJump && groundCheck1)	// played jumping sound, now check if he has hit the ground(landed)
	{
		//gameMusic->playCollisionSound("Player", "Ground"); // Not sure if I like this collision sound or if it's necessary
		soundedJump = false;
	}
}
//-------------------------------------------------------------------------------------
void Simulator::setCamera(PlayerCamera* pcam)
{
	if (Ogre::StringUtil::match(pcam->name, "P1Cam", true))
		this->player1Cam = pcam;
	if (Ogre::StringUtil::match(pcam->name, "P2Cam", true))
		this->player2Cam = pcam;
}
//-------------------------------------------------------------------------------------
void Simulator::setPlayer(Player* p)
{
	if (Ogre::StringUtil::match(p->getGameObjectName(), "Player1", true))
		this->p1 = p;
	if (Ogre::StringUtil::match(p->getGameObjectName(), "Player2", true))
		this->p2 = p;
}
//-------------------------------------------------------------------------------------
PlayerCamera* Simulator::getPlayerCamera(Ogre::String name)
{
	if (Ogre::StringUtil::match(name, "P1Cam", true))
		return this->player1Cam;
	if (Ogre::StringUtil::match(name, "P2Cam", true))
		return this->player2Cam;
}
//-------------------------------------------------------------------------------------
void Simulator::toggleViewChange(Ogre::String name)
{
	if (Ogre::StringUtil::match(name, "Player1", true))
		viewChangeP1 = !viewChangeP1;
	if (Ogre::StringUtil::match(name, "Player2", true))
		viewChangeP2 = !viewChangeP2;
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
 		printf("p1 throwing!\n\n");

	if (throwFlag) // Add disk back to simulator and it will take care of throw velocity
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
		if (p->getGameObjectName() == "Player1")
		{
			player1CanCatch = false;
			player2CanCatch = true;
			playerLastThrew = p->getGameObjectName();
		}
		if (p->getGameObjectName() == "Player2")
		{
			player1CanCatch = true;
			player2CanCatch = false;
			playerLastThrew = p->getGameObjectName();
		}

    	p->setHolding();
    }
    else // Update position relative to the Player
    {
    	Ogre::Vector3 dpos;
    	float newDiskZ= -p->getPlayerDimensions().z;
    	if(p->getGameObjectName() == "Player2")
    			newDiskZ = p->getPlayerDimensions().z;

    	dpos = p->getSceneNode()->getOrientation() * Ogre::Vector3(0.0f, 0.0f, newDiskZ);
		gameDisk->getSceneNode()->_setDerivedPosition(dpos + p->getSceneNode()->getPosition());

		Ogre::Vector3 dpos_derived = gameDisk->getSceneNode()->_getDerivedPosition();
		diskOrientation = btQuaternion(0, 0, 0);
		transform = btTransform(diskOrientation, btVector3(dpos_derived.x, dpos_derived.y, dpos_derived.z));
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
void Simulator::handleDiskCollisions(GameObject* disk, GameObject* o)
{
	// Wall
	if (o->typeName == "Wall")
	{	
		if (!p1->checkHolding())
			wallHitAfterThrow = true;
		if (p2 != NULL)
			if (p2->checkHolding())
				wallHitAfterThrow = true;
		if (previousWallHit == "NULL")
		{
			// gameDisk->needsOrientationUpdate = true;
			previousWallHit = o->getGameObjectName();
			adjustDiskOrientation(gameDisk, gameDisk->getBody()->getLinearVelocity(), previousWallHit);
			gameMusic->playCollisionSound("Disk", "Wall");
		}
		else if (previousWallHit != o->getGameObjectName())
		{
			// gameDisk->needsOrientationUpdate = true;
			previousWallHit = o->getGameObjectName();
			adjustDiskOrientation(gameDisk, gameDisk->getBody()->getLinearVelocity(), previousWallHit);
			gameMusic->playCollisionSound("Disk", "Wall");
		}

		if (!player1CanCatch && !p1->checkHolding())
			player1CanCatch = true;
		if (p2 != NULL)
		{
			if (!player2CanCatch && !p2->checkHolding())
				player2CanCatch = true;
		}
	}
	// Player
	else if (o->typeName == "Player")
	{
		if (((Player*)o)->checkHolding() == false)
		{
			if (player1CanCatch && ((Player*)o)->getGameObjectName() == "Player1")
			{
				((Player*)o)->attachDisk((Disk*)disk);
				gameMusic->playCollisionSound("Disk", "Player");
			}
			if (player2CanCatch && ((Player*)o)->getGameObjectName() == "Player2")
			{
				((Player*)o)->attachDisk((Disk*)disk);
				gameMusic->playCollisionSound("Disk", "Player");
			}
			gameStart = true;
			gameMusic->playCollisionSound("Disk", "Player");
		}
	}
	// Target
	else if (o->typeName == "Target")
	{
		if (((Target*)o)->checkHitFlag() == false)
		{
			printf("COLLIDED WITH TILE!\n\n\n");

			// The 47.0f value is the x-width and y-height of the disk
		    ((Target*)o)->toggleHitFlag();
			removeObject(o->getGameObjectName());
			if (o->getGameObjectName() == "Power" || o->getGameObjectName() == "Speed" || o->getGameObjectName() == "Jump" || o->getGameObjectName() == "Restore") 
			{
			    o->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
								    ,getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
							       Ogre::Math::RangeRandom(getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
								    ,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
							       Ogre::Math::RangeRandom(-5.0f, 5.0f));
			    if (gameDisk->activatePowerUp(o->getGameObjectName(), (Player*)getGameObject(playerLastThrew)))
			        restoreTile();
                gameMusic->playCollisionSound("Disk", "Target");
                if (gameDisk->powerUp == "Speed")
                    gameMusic->playMusic("SpeedUp");
			}
			else
			{		    
			    o->getSceneNode()->setPosition(Ogre::Math::RangeRandom(getGameObject("LeftWall")->getSceneNode()->getPosition().x + (1.0f/2.0f)
								    ,getGameObject("RightWall")->getSceneNode()->getPosition().x - (1.0f/2.0f)), 
							       Ogre::Math::RangeRandom(getGameObject("client11")->getSceneNode()->getPosition().y + (2.0f/3.0f)
								    ,getGameObject("Ceiling")->getSceneNode()->getPosition().y - (2.0f/3.0f)), 
							       Ogre::Math::RangeRandom(getGameObject("Ceiling")->getSceneNode()->getPosition().z
								    ,getGameObject("FarWall")->getSceneNode()->getPosition().z));
			    gameMusic->playCollisionSound("Disk", "Target");
		    }
			o->addToSimulator();
		}
	}
	else if (o->typeName == "Tile" && !((Tile *)o)->checkHitFlag() && wallHitAfterThrow && !p1->checkHolding())
	{	
		((Tile *)o)->toggleHitFlag(); // Mark that the tile has been hit
		removeObject(((Tile*)o)->getGameObjectName());

		int index = ((Tile *)o)->indexIntoTileArray;
		
		printf("COLLIDED WITH TILE!\n\n\n");
		if (((Tile*)hostTileList[((Tile *)o)->indexIntoTileArray])->getGameObjectName() == o->getGameObjectName())
		    destroyTiles(hostTileList, hostRemoveIndexes, index);
		else
		    destroyTiles(clientTileList, clientRemoveIndexes, index);
		newRemovedTile = true;
		gameDisk->resetPowerUp();
		((Player*)getGameObject(playerLastThrew))->attachDisk((Disk*)disk);
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
void Simulator::updatePlayerCamera(PlayerCamera* cam, const Ogre::Real elapseTime)
{
	if (cam->name == "P1Cam")
	{
		if (viewChangeP1) // View was toggled; now check what view it needs to be changed to
		{
			toggleViewChange(p1->getGameObjectName()); // want to set toggle flag back since you are now either entering or leaving Aim View

			if(player1Cam->isInAimMode()) // Go into Aim view
				player1Cam->initializePosition(((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			else // Return from Aim view
				player1Cam->initializePosition(p1->getPlayerCameraNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
		}
		else  // No toggle, so just update the position of the camera; need to add an if for AimMode rotation
		{
			if (player1Cam->isInAimMode())
				player1Cam->update(elapseTime, ((GameObject*)p1)->getSceneNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());
			else
				player1Cam->update(elapseTime, p1->getPlayerCameraNode()->_getDerivedPosition(), p1->getPlayerSightNode()->_getDerivedPosition());			
		}
	}
}
//-------------------------------------------------------------------------------------
bool Simulator::checkGameStart()
{
	return gameStart;
}
//---------------------------------------------------------------------------------------
void Simulator::restoreTile() {
    if (p1 != NULL && p1->getGameObjectName() == playerLastThrew && hostRemoveIndexes.size() > 0) {
        hostTileList[hostRemoveIndexes.back()]->addToSimulator();
        hostRemoveIndexes.pop_back();
        gameMusic->playMusic("Restore");
    }
    else if (p2 != NULL && p2->getGameObjectName() == playerLastThrew && clientRemoveIndexes.size() > 0) {
        clientTileList[clientRemoveIndexes.back()]->addToSimulator();
        clientRemoveIndexes.pop_back();
        gameMusic->playMusic("Restore");
    }
}
//-------------------------------------------------------------------------------------
void Simulator::destroyTiles(vector<GameObject*>& tileList, vector<int>& removeIndexes, int index) {
    removeIndexes.push_back(index);
    /* When removing from client - add to clientRemoveIndexes */
    if (gameDisk->powerUp == "Power") {
        int col = index % 6;
        if (col != 0) { // remove left - not in far left column
            if (!(tileList[index-1]->checkHitFlag())) {
                tileList[index-1]->toggleHitFlag();
                removeIndexes.push_back(index-1);
                removeObject(tileList[index-1]->getGameObjectName());
            }
        }
        if (col != 5) { // remove right - not in far right column
            if (!(tileList[index+1]->checkHitFlag())) {
                tileList[index+1]->toggleHitFlag();
                removeIndexes.push_back(index+1);
                removeObject(tileList[index+1]->getGameObjectName());
            }
        }
        if (index > 5) { // remove top - not in top row
            if (!(tileList[index-6]->checkHitFlag())) {
                tileList[index-6]->toggleHitFlag();
                removeIndexes.push_back(index-6);
                removeObject(tileList[index-6]->getGameObjectName());
            }
        }
        if (index < 36) { // remove bottom - not in bottom row
            if (!(tileList[index+6]->checkHitFlag())) {
                tileList[index+6]->toggleHitFlag();
                removeIndexes.push_back(index+6);
                removeObject(tileList[index+6]->getGameObjectName());
            }
        }
        gameMusic->playMusic("BigBlast");
    }
    else
        gameMusic->playMusic("Blast");
}
GameObject* Simulator::objListCheck(int index)
{
	return objList[index];
}