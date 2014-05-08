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
    pCamera->setPosition(mCameraNode->getPosition());

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
    
    if (!inAimMode) // Restore SightNode to default position when the player leaves Aim View
    {
        overlay->hide();
        overlay2->hide();
        p->getSceneNode()->setVisible(true);
        if (p->getPlayerDisk() != NULL)
            p->getPlayerDisk()->getSceneNode()->setVisible(true);
    }
    else
    {
        overlay->show();
        overlay2->show();
        p->getSceneNode()->setVisible(false);
        if (p->getPlayerDisk() != NULL)
            p->getPlayerDisk()->getSceneNode()->setVisible(false);
    }
    p->particleNode->setVisible(p->jumpPowerActive);
}
//-------------------------------------------------------------------------------------
void PlayerCamera::initializePosition(Ogre::Vector3 cameraPosition, Ogre::Vector3 sightPosition)
{
    mCameraNode->setPosition(cameraPosition);/* + Ogre::Vector3(0.0f, 0.0f, 5.0f));*/
    mTargetNode->setPosition(sightPosition);
}
//-------------------------------------------------------------------------------------
void PlayerCamera::update (Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition) 
{   // pCam->update(clientPlayer->getPlayerCameraNode()->_getDerivedPosition(), clientPlayer->getPlayerSightNode()->_getDerivedPosition());
    Ogre::Vector3 displacement;

    mCameraNode->_setDerivedPosition(cameraPosition);/* + Ogre::Vector3(0.0f, 0.0f, 5.0f));*/
    mTargetNode->_setDerivedPosition(targetPosition);
 /*
    displacement = (cameraPosition - mCameraNode->getPosition());
    // printf("Camera displacement = %f, %f, %f\n", displacement.x, displacement.y, displacement.z);
    mCameraNode->translate(displacement);
 
    displacement = (targetPosition - mTargetNode->getPosition());
    // printf("Sight displacement = %f, %f, %f\n", displacement.x, displacement.y, displacement.z);
    mTargetNode->translate(displacement);
 */   // printf("\tSight Position = %f, %f, %f\n", mTargetNode->getPosition().x, mTargetNode->getPosition().y, mTargetNode->getPosition().z);
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
