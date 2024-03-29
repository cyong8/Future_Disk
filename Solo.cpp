#include "Solo.h"

Solo::Solo(MCP* mcp)//Music* mus, Ogre::SceneManager* mgr)
{
	MasterControl = mcp;
	gameMusic = mcp->getMusicObject();
	sceneMgr = mcp->getSceneManager();

	gameSimulator = new Simulator(sceneMgr, gameMusic);

    score = 0;

    srand(time(0));
    time(&gapStartTime);
    time(&gapEndTime);

	timeSinceLastStateUpdate = 0.01f;
    mMove = 5.0f;
    mRotate = 0.1f;
    sprintFactor = 2.0f;
    gameDisk = NULL;
    gameStart = false;
    gameOver = false;
    gamePause = false;
    diskAdded = false;
    boostPenalty = false;
    target_list = vector<Target*>(NUM_OF_TARGETS, NULL);

    time(&initTime);
    initMinutes = 2;
    pTimePassed = 0;

    createScene();
}
//-------------------------------------------------------------------------------------
Solo::~Solo(void)
{
    
}
//-------------------------------------------------------------------------------------
void Solo::createScene()
{
    gameRoom = new Room(sceneMgr, gameSimulator, 2);

    for (int i = 0; i < NUM_OF_TARGETS; i++)
    {
        Target *target = new Target("Target_" + Ogre::StringConverter::toString(i), sceneMgr, gameSimulator, Ogre::Vector3(2.5f, 0.01f, 2.5f), gameRoom, TARGET, i);
        target->addToSimulator();
        target_list.push_back(target);
    }

    // Lights
    sceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));  // Ambient light
    sceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    
    pointLight = sceneMgr->createLight("roomLight");  // Point light
    pointLight->setType(Ogre::Light::LT_POINT);
    pointLight->setDiffuseColour(Ogre::ColourValue::White);
    pointLight->setSpecularColour(Ogre::ColourValue::White);
    pointLight->setVisible(true);
    pointLight->setPosition(Ogre::Vector3(0.0f, gameRoom->getWall(Ceiling)->getSceneNode()->getPosition().y, 0.0f));

    gameRoom->activateRoom();

    player = new Player("Player1", sceneMgr, gameSimulator, Ogre::Vector3(1.3f, 1.3f, 1.3f), 1, gameRoom, MasterControl->gui);
    player->addToSimulator();
    
    pCam = new PlayerCamera("soloCamera", sceneMgr, sceneMgr->getCamera("PlayerCam"));/*need camera object*/
    pCam->initializePosition(player->getPlayerCameraNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());
    pCam->setPlayer(player);
    
    gameStart = true;

    createOverlays(pCam);
}
//-------------------------------------------------------------------------------------
bool Solo::frameRenderingQueued(const Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    // else if (!gameStart) // may need
    //     gameStart = true;
    //bool ret = BaseApplication::frameRenderingQueued(evt);
    if(player->getCustomAnimationState() != NULL)
        player->getCustomAnimationState()->addTime(tSinceLastFrame);
    
    if(gameDisk != NULL && gameDisk->diskAnimationState != NULL)
        gameDisk->diskAnimationState->addTime(tSinceLastFrame*2);

    if(!gameStart && !gameOver) // Game not started
    {
        MasterControl->gui->removeLabel(MasterControl->getLabel(PAUSE));
        MasterControl->gui->removePanel(MasterControl->getPanel(GAMEOVER));
        printf("!gamestart && !gameOver");
    }
    else if(gameOver)
    {
        if (gameStart) 
        {
            MasterControl->gui->addPanel(MasterControl->getPanel(GAMEOVER), OgreBites::TL_CENTER);
            MasterControl->getPanel(GAMEOVER)->setParamValue(1, Ogre::StringConverter::toString(score));
            printf("gameOver");
            MasterControl->gui->gameOverScreen();
            gameStart = false;
        }

        // Add option to restart game
    }
    else // Game started
    {
        if(!gamePause)
        {
            gameSimulator->stepSimulation(tSinceLastFrame, 1, 1.0f/60.0f);
            gameSimulator->parseCollisions(); // check collisions
            time_t currTime;
            time(&currTime);
            gameOver = updateTimer(currTime);
            if (gameOver)
                gameMusic->playMusic("Pause");
            modifyScore(gameSimulator->tallyScore());
        }
        else
        {
            time_t pcurrTime;
            time(&pcurrTime);
            updatePauseTime(pcurrTime);
        }
    }

    processUnbufferedInput(tSinceLastFrame, mKeyboard, mMouse);
    restrictPlayerMovement();

    if (gameSimulator->checkDiskSet() && !diskAdded)
    {
        if (gameDisk == NULL) 
        {
            gameDisk = new Disk("Disk", sceneMgr, gameSimulator, 1.0f/*Ogre::Math::RangeRandom(0,2)*/, 1);
            gameDisk->diskAnimationState = gameDisk->diskEnt->getAnimationState("spin");
            gameDisk->diskAnimationState->setEnabled(true);
            gameDisk->diskAnimationState->setLoop(true);
            gameDisk->diskAnimationState->setTimePosition(0);
        }
        gameDisk->addToSimulator();
        diskAdded = true;
    }

    updateCamera(tSinceLastFrame);
}
//-------------------------------------------------------------------------------------
void Solo::updateCamera(Ogre::Real elapseTime)
{
    if (pCam->isInAimMode())
        pCam->update(player->getSceneNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());
    else
        pCam->update(player->getPlayerCameraNode()->_getDerivedPosition(), player->getPlayerSightNode()->_getDerivedPosition());      
}
//-------------------------------------------------------------------------------------
bool Solo::mouseMoved(Ogre::Real relX, Ogre::Real relY)
{
    if (gamePause || !gameStart)
        return true;
    if (!gameSimulator->checkGameStart())
        return true;

    Ogre::SceneNode* pSceneNode = player->getSceneNode();
    Ogre::SceneNode* pSightNode = player->getPlayerSightNode();
    Ogre::SceneNode* pCamNode = player->getPlayerCameraNode();
    Ogre::Vector3 sightHeight;
    btRigidBody* pBody;
    btTransform transform;
    btQuaternion rotationQ;

    pBody = player->getBody();
    transform = pBody->getCenterOfMassTransform();

    if (pCam->isInAimMode())
    {   
        pSceneNode->yaw(Ogre::Degree((-mRotate /2) * relX), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -relY, 0.0f);
    }
    else
    {
        pSceneNode->yaw(Ogre::Degree(-mRotate * relX), Ogre::Node::TS_WORLD);
        sightHeight = Ogre::Vector3(0.0f, -relY, 0.0f);
    }

    rotationQ = btQuaternion(pSceneNode->getOrientation().getYaw().valueRadians(), 0, 0);
    transform.setRotation(rotationQ);
    pBody->setCenterOfMassTransform(transform);
    
    pSightNode->setPosition(pSightNode->getPosition() + sightHeight);
    // pCamNode->setPosition(Ogre::Vector3(pCamNode->getPosition().x, -pSightNode->getPosition().y, pCamNode->getPosition().z));
    pCamNode->setPosition(Ogre::Vector3(pCamNode->getPosition().x, pCamNode->getPosition().y, -pSightNode->getPosition().z));

    return true;
}
//-------------------------------------------------------------------------------------
bool Solo::processUnbufferedInput(const Ogre::Real tSinceLastFrame, OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
{
    /********************  KEY VARIABLES ********************/    
    static bool mMouseDown = false;                                    // If a mouse button is depressed
    static bool pausePressedLast = false;                              // Was pause pressed last frame
    static bool spacePressedLast = false;
    static Ogre::Real timeSinceLastJump = 0.0f;
    bool turboMode = false;
    bool keyWasPressed = false;                                        // Was a key pressed in current frame
    bool currMouse = mMouse->getMouseState().buttonDown(OIS::MB_Left); // Current state of the mouse

    static bool vKeyDown = false;

    timeSinceLastJump += tSinceLastFrame;

    /********************     MOVEMENT   ********************/
    // Allow movement if the player is on the floor and the game is not paused
    if(!gamePause && gameSimulator->checkGameStart())
    {
        // If the mouse button was not pressed in the last frame, the mouse is pressed in the current frame, and the player is holding the disk then they are trying to throw
        if(!mMouseDown && currMouse && player->checkHolding() && vKeyDown) 
        {
            gameMusic->playMusic("Throw");
            gameSimulator->setThrowFlag();
            player->getPlayerDisk()->getSceneNode()->setVisible(true, true);
            player->animateCharacter("throw");
        }
        mMouseDown = currMouse; // Set that the mouse WAS pressed
        
        if (mKeyboard->isKeyDown(OIS::KC_V) && !vKeyDown) // if 'v' is pressed and was not pressed last frame - go to aim mode
        {
            pCam->toggleThirdPersonView();
            vKeyDown = true;
            //showTrajectory(pc);
        }
        if (!mKeyboard->isKeyDown(OIS::KC_V) && vKeyDown) // if 'v' is not pressed and was pressed last frame - exit aim mode
        {
            pCam->toggleThirdPersonView();
            vKeyDown = false;
            //mSceneMgr->getRootSceneNode()->detachObject(trajectory);
            //trajectory->clear();
        }
        /*newTime = clock();
        if(mKeyboard->isKeyDown(OIS::KC_LSHIFT)) // Move into Boost mode
        {
            if (player->getRemainingTime() > 0.0f) {
                clock_t t = clock();
                remainingTime 
                player->setState(BOOST, true);
            }
            else 
                player->setState(BOOST, false);
        }
        else
        {
            if (player->getRemainingTime() < 4.0f)
                remainingTime += (float)newTime/CLOCKS_PER_SEC - (float)oldTime/CLOCKS_PER_SEC;
            else
                remainingTime = 4.0f;
            player->setState(BOOST, false);
        }
        oldTime = newTime;*/
        
        // If the 'V' key is down you shouldn't be able to move
        if (!vKeyDown)  
        {
            if (mKeyboard->isKeyDown(OIS::KC_LSHIFT) && !boostPenalty)
            {
                boostPenalty = player->updateBoost(true);
            }
            else
            {
                boostPenalty = player->updateBoost(false);
            }
            if (!mKeyboard->isKeyDown(OIS::KC_W) && !mKeyboard->isKeyDown(OIS::KC_A) 
            && !mKeyboard->isKeyDown(OIS::KC_S) && !mKeyboard->isKeyDown(OIS::KC_D)) 
            {
                if(player->moving)
                {
                    player->nullAnimationState();
                }
                player->moving = false;
            }
            else
            {
                if(!player->moving)
                    player->animateCharacter("walk");
                player->moving = true;
            }

             // Move the player
            if (mKeyboard->isKeyDown(OIS::KC_W)) // Forward
            {
                player->setState(FORWARD, true);
            }
            else
                player->setState(FORWARD, false);


            if (mKeyboard->isKeyDown(OIS::KC_S)) // Backward
            {
                player->setState(BACK, true);
            } 
            else
                player->setState(BACK, false);

            if (mKeyboard->isKeyDown(OIS::KC_A)) // Left - yaw or strafe
            {
                player->setState(LEFT, true);              
            } 
            else
                player->setState(LEFT, false);

            if (mKeyboard->isKeyDown(OIS::KC_D)) // Right - yaw or strafe
            {
                player->setState(RIGHT, true);
            } 
            else
                player->setState(RIGHT, false);

            if (mKeyboard->isKeyDown(OIS::KC_SPACE) && !spacePressedLast)// && !p->groundConstantSet && !spacePressedLast) 
            {
                if(player->performJump())
                {
                    if (player->jumpPowerActive)
                        gameMusic->playMusic("SuperJump");
                    else
                        gameMusic->playMusic("Jump");

                    spacePressedLast = false;
                    player->animateCharacter("jump");

                    spacePressedLast = true;
                    gameSimulator->soundedJump = true;
                }
            }
            if (!mKeyboard->isKeyDown(OIS::KC_SPACE) && spacePressedLast)
            {
                spacePressedLast = false;
            }
            if(!player->checkMovementRestriction())
            {   // Rotate the velocity vector by the orientation of the player
                
                Ogre::Vector3 velocityVector =  player->getSceneNode()->_getDerivedOrientation() * player->fillVelocityVector(mMove, sprintFactor);
                
                btVector3 btTrueVelocity = btVector3(velocityVector.x, velocityVector.y, velocityVector.z);            
                player->getBody()->setLinearVelocity(btTrueVelocity + (btVector3(0.0f, player->getBody()->getLinearVelocity().getY(), 0.0f)));
            }
        }
    }
}
//-------------------------------------------------------------------------------------
void Solo::togglePause()
{
    MasterControl->gui->pauseMenu(gamePause);
    
    if (gamePause == true)  //leaving pause
    {        
        gameMusic->playMusic("Play");
        gamePause = false;
        MasterControl->gui->removeLabel(MasterControl->getLabel(PAUSE));
        //MasterControl->gui->removePanel(MasterControl->getPanel(OBJECTIVE));
        //MasterControl->gui->removePanel(MasterControl->getPanel(INSTRUCT));
    }
    else //entering Pause
    {        
        gameMusic->playMusic("Start");
        //MasterControl->getLabel(PAUSE)->setCaption("GAME PAUSED!");
        //MasterControl->gui->addLabel(MasterControl->getLabel(PAUSE), OgreBites::TL_CENTER);
        //MasterControl->gui->addPanel(MasterControl->getPanel(OBJECTIVE), OgreBites::TL_BOTTOM);
        //MasterControl->gui->addPanel(MasterControl->getPanel(INSTRUCT), OgreBites::TL_RIGHT);
        gamePause = true;
        time(&pauseTime);
    }
} 
//-------------------------------------------------------------------------------------
void Solo::createOverlays(PlayerCamera* playCam) // might move to Client and Server
{
    /********************    MENUS    ********************/
    Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
    
    Ogre::Overlay* crossHairVertOverlay = overlayManager->create("crossHairVert"); // Create an overlay for the vertical crosshair

    // Create an overlay container for the vertical crosshair
    Ogre::OverlayContainer* crossHairVertContainer = static_cast<Ogre::OverlayContainer*>( overlayManager->createOverlayElement("Panel", "VerticalPanel"));
    crossHairVertContainer->setPosition(0.5f, 0.4f);
    crossHairVertContainer->setDimensions(0.001f, 0.2f);
    crossHairVertContainer->setMaterialName("BaseWhite");
    crossHairVertContainer->getMaterial()->setReceiveShadows(false);

    crossHairVertOverlay->add2D( crossHairVertContainer ); // Add crossHairVertContainer to the crossHairVertOverlay

    Ogre::Overlay* crossHairHorizOverlay = overlayManager->create("crossHairHoriz"); // Create an overlay for the horizontal crosshair

    // Create an overlay container for the horizontal crosshair
    Ogre::OverlayContainer* crossHairHorizContainer = static_cast<Ogre::OverlayContainer*>(overlayManager->createOverlayElement("Panel", "HorizontalPanel"));
    crossHairHorizContainer->setPosition(0.425, 0.5);
    crossHairHorizContainer->setDimensions(0.15, 0.001);
    crossHairHorizContainer->setMaterialName("BaseWhite");
    crossHairHorizContainer->getMaterial()->setReceiveShadows(false);

    crossHairHorizOverlay->add2D(crossHairHorizContainer);     // Add the crossHairHorizContainer to the crossHairHorizOverlay

    crossHairVertOverlay->hide();    // Hide the Crosshair till 
    crossHairHorizOverlay->hide();   // til Aim View is activated 

    playCam->setCHOverlays(crossHairVertOverlay, crossHairHorizOverlay);
}
//-------------------------------------------------------------------------------------
void Solo::restrictPlayerMovement()
{
    Ogre::Vector3 pos = player->getSceneNode()->getPosition();
    Ogre::Vector3 dim = player->getPlayerDimensions();

    btVector3 velocityVector = player->getBody()->getLinearVelocity();
    Ogre::Real pushBackVelocity = 5.0f;

    Ogre::SceneNode* restrictHNode;
    Ogre::SceneNode* restrictVNode;

    Ogre::AxisAlignedBox gapHBox;
    Ogre::AxisAlignedBox gapVBox;
    Ogre::AxisAlignedBox playerBox = player->getSceneNode()->_getWorldAABB();


    RoomSpace* gp = gameRoom->getPlayerRoomSpace(player->getPlayerID());

    restrictHNode = gp->horizontalGap;
    gapHBox = restrictHNode->_getWorldAABB();

    // restrictVNode = gp->vGap;
    // gapVBox = restrictVNode->_getWorldAABB();


    if (gapHBox.intersects(playerBox))
    {
        time(&gapStartTime);
        time(&gapEndTime);

        if (player->getPlayerID() > 2)
            pushBackVelocity = -pushBackVelocity;

        player->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        player->getBody()->setLinearVelocity(btVector3(velocityVector.getX(), velocityVector.getY(), pushBackVelocity));
        restrictHNode->setVisible(true);
        player->setMovementRestriction(true);
    }
    // else if (gapVBox.intersects(playerBox))
    // {
    //     time(&gapStartTime);
    //     time(&gapEndTime);

    //     if (player->getPlayerID() == 1 || player->getPlayerID() == 3)
    //         pushBackVelocity = -pushBackVelocity;

    //     player->getBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
    //     player->getBody()->setLinearVelocity(btVector3(pushBackVelocity, velocityVector.getY(), velocityVector.getZ()));
    //     restrictVNode->setVisible(true);
    //     player->setMovementRestriction(true);
    // }
    else
    {
        time(&gapEndTime);
        // gapEndClock = clock();

        if (difftime(gapEndTime, gapStartTime) > 1.0f)
        {
            restrictHNode->setVisible(false);
            // restrictVNode->setVisible(false);
        }

        player->setMovementRestriction(false);
    }
}
//-------------------------------------------------------------------------------------
bool Solo::updateTimer(time_t currTime)
{
    double secondsElapsed = difftime(currTime, initTime);
    int secondsLeft = (initMinutes*60) - secondsElapsed + pTimePassed;

    int minutes = secondsLeft / 60;
    int seconds = secondsLeft % 60;

    Ogre::String mins = Ogre::StringConverter::toString(minutes);
    Ogre::String sec = Ogre::StringConverter::toString(seconds);

    if(minutes < 10)
        mins = "0"+mins;

    if(minutes <= 0)
        mins = "00";

    if(seconds < 10)
        sec = "0"+sec;

    if(seconds <= 0)
        sec = "00";
    
    MasterControl->getPanel(SCORE)->setParamValue(0, Ogre::StringConverter::toString(score));
    MasterControl->getPanel(SCORE)->setParamValue(1, mins + ":" + sec);
    if(minutes <= 0 && seconds <= 0)
        return true;
    return false;
}
//-------------------------------------------------------------------------------------
void Solo::updatePauseTime(time_t currTime)
{
    pTimePassed = difftime(currTime, pauseTime);
}
//-------------------------------------------------------------------------------------
void Solo::restartGame()
{	
    gameSimulator->resetSimulator();
    //gameSimulator->stepSimulation(evt.timeSinceLastFrame, 1, 1.0f/60.0f);

    diskAdded = false;
    player->addToSimulator();
    
    //gameSimulator->removeObject(player->getGameObjectName());
	
	gameStart = true;
	gameOver = false;
	if (gamePause)
	    togglePause();
	time(&initTime);
	
	MasterControl->gui->removePanel(MasterControl->getPanel(GAMEOVER));
	
	score = 0;
    
    //player->addToSimulator();
}    
//-------------------------------------------------------------------------------------
void Solo::modifyScore(int increase)
{
    score += increase;
}
