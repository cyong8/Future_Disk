#ifndef __PlayerCamera_h_
#define __PlayerCamera_h_

#include "BaseApplication.h"

class Player;

class PlayerCamera
{

public: 
	Player* p;
	Ogre::SceneNode *CameraNode;

	PlayerCamera(void);
	virtual ~PlayerCamera(void);

	virtual void createCamera(void);
	void switchToThirdView(void);
	void switchToFirstView(void);
};



#endif // #ifndef __PlayerCamera_h_