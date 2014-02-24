#include "PlayerCamera.h"
#include "Player.h"

PlayerCamera::PlayerCamera(Ogre::String nym, Ogre::SceneManager *mgr, Ogre::Camera* pCamera)
{
    name = nym;
    mCamSceneMgr = mgr;
    mCamera = pCamera;
    inAimMode = false;

    // create nodes for cameras
    mCameraNode = mCamSceneMgr->getRootSceneNode()->createChildSceneNode(nym);
    mTargetNode = mCamSceneMgr->getRootSceneNode()->createChildSceneNode(nym + "_target");

    mCameraNode->setAutoTracking(true, mTargetNode); // The camera will always look at the camera target
    mCameraNode->setFixedYawAxis(true); // Needed because of auto tracking

    mCameraNode->attachObject(pCamera); // attach Ogre::Camera to camera node
}

PlayerCamera::~PlayerCamera(void)
{
	mCameraNode->detachAllObjects();
    mCamSceneMgr->destroySceneNode(name);
    mCamSceneMgr->destroySceneNode(name + "_target");
}

void PlayerCamera::toggleThirdPersonView(void)
{
    // Implement by translation of the CameraNode of the player class, toggle visibility of player and targetnode
    inAimMode = !inAimMode;    
    //camera position is changed in stepSimulation.
}

void PlayerCamera::initializePosition(Ogre::Vector3 cameraPosition, Ogre::Vector3 sightPosition)
{
    mCameraNode->setPosition(cameraPosition);
    mTargetNode->setPosition(sightPosition);
}

void PlayerCamera::update (Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition) 
{
    // Handle movement
    Ogre::Vector3 displacement;
 
    displacement = (cameraPosition - mCameraNode->getPosition());
    mCameraNode->translate(displacement);
 
    displacement = (targetPosition - mTargetNode->getPosition());
    mTargetNode->translate(displacement);
}

void PlayerCamera::setPlayer(Player* player)
{
    p = player;
}

bool PlayerCamera::isInAimMode()
{
    return inAimMode;
}

Ogre::Camera* PlayerCamera::getMCamera()
{
    return mCamera;
}