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
//-------------------------------------------------------------------------------------
PlayerCamera::~PlayerCamera()
{
	mCameraNode->detachAllObjects();
    mCamSceneMgr->destroySceneNode(name);
    mCamSceneMgr->destroySceneNode(name + "_target");
}
//-------------------------------------------------------------------------------------
void PlayerCamera::toggleThirdPersonView()
{
    inAimMode = !inAimMode;    
 
    //p->getPlayerSightNode()->setPosition(Ogre::Vector3(0.0f, 0.0f, -400.0f)); // reset camera to face in front of player
    //mTargetNode->setPosition(p->getPlayerSightNode()->getPosition());
    if (!inAimMode) // Restore SightNode to default position when the player leaves Aim View
    {
        overlay->hide();
        overlay2->hide();
        p->getSceneNode()->setVisible(true, true);
    }
    else
    {
        overlay->show();
        overlay2->show();
        p->getSceneNode()->setVisible(false);
    }
}
//-------------------------------------------------------------------------------------
void PlayerCamera::initializePosition(Ogre::Vector3 cameraPosition, Ogre::Vector3 sightPosition)
{
    mCameraNode->setPosition(cameraPosition);
    mTargetNode->setPosition(sightPosition);
}
//-------------------------------------------------------------------------------------
void PlayerCamera::update (Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition) 
{
    // Handle movement
    Ogre::Vector3 displacement;
 
    displacement = (cameraPosition - mCameraNode->getPosition());
    mCameraNode->translate(displacement);
 
    displacement = (targetPosition - mTargetNode->getPosition());
    mTargetNode->translate(displacement);
}
//-------------------------------------------------------------------------------------
void PlayerCamera::setPlayer(Player* player)
{
    p = player;
}
//-------------------------------------------------------------------------------------
Player* PlayerCamera::getPlayer()
{
    return p;
}
//-------------------------------------------------------------------------------------
bool PlayerCamera::isInAimMode()
{
    return inAimMode;
}
//-------------------------------------------------------------------------------------
Ogre::Camera* PlayerCamera::getMCamera()
{
    return mCamera;
}
//-------------------------------------------------------------------------------------
void PlayerCamera::setCHOverlays(Ogre::Overlay* o, Ogre::Overlay* o2)
{
    overlay = o;
    overlay2 = o2;
}
//-------------------------------------------------------------------------------------
Ogre::SceneNode* PlayerCamera::getPCamSceneNode()
{
    return mCameraNode;
}
