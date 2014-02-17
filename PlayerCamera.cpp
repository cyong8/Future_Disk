#include "PlayerCamera.h"

PlayerCamera::PlayerCamera(void)
{
}

PlayerCamera::~PlayerCamera(void)
{
	delete mRoot;
}

void PlayerCamera::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    CameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerCamNode");
    CameraNode.attachObject(mCamera);


    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,0,80));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,-300));
    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void PlayerCamera::switchToThirdView(void)
{

}

void PlayerCamera::switchToFirstView(void)
{
    
}
