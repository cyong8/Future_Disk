#ifndef __PlayerCamera_h_
#define __PlayerCamera_h_

#include "BaseApplication.h"

class Player;
// Framework implemented from: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=3rd+person+camera+system+tutorial
class PlayerCamera
{
protected:
    Ogre::SceneNode *mTargetNode; // The camera target
    Ogre::SceneNode *mCameraNode; // The camera itself
    Ogre::Camera *mCamera; // Ogre camera
    Ogre::SceneManager *mCamSceneMgr;
    Player* p;

public: 
	PlayerCamera(Ogre::String nym, Ogre::SceneManager *mgr, Ogre::Camera *camera);
	virtual ~PlayerCamera(void);
	void initializePosition(Ogre::Vector3 iV);
    void update(Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition);
    void switchToThirdView(void);
    
	Ogre::String name;
};



#endif // #ifndef __PlayerCamera_h_