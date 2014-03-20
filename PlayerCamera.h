#ifndef __PlayerCamera_h_
#define __PlayerCamera_h_

#include "BaseApplication.h"
#include "MCP.h"

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
    bool inAimMode;

public: 
	PlayerCamera(Ogre::String nym, Ogre::SceneManager *mgr, Ogre::Camera *camera);
	virtual ~PlayerCamera(void);
	void initializePosition(Ogre::Vector3 cameraPosition, Ogre::Vector3 sightPosition);
    void update(Ogre::Real elapsedTime, Ogre::Vector3 cameraPosition, Ogre::Vector3 targetPosition);
    void toggleThirdPersonView(void);
    void setPlayer(Player* player);
    Player* getPlayer(void);
    bool isInAimMode(void);
    Ogre::Camera* getMCamera(void);
    void setCHOverlays(Ogre::Overlay* o, Ogre::Overlay* o2);
    Ogre::SceneNode* getPCamSceneNode(void);

	Ogre::String name;
    Ogre::Overlay* overlay;
    Ogre::Overlay* overlay2;
};



#endif // #ifndef __PlayerCamera_h_
