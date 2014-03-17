#ifndef __OgreMotionState_h_
#define __OgreMotionState_h_

#include "BaseApplication.h"

class OgreMotionState : public btMotionState
{
protected:
	Ogre::SceneNode* mVisibleobj;
	btTransform mPos1;
	GameObject* gObj;

public:
	OgreMotionState(const btTransform& initialpos, Ogre::SceneNode* node)
	{
		mVisibleobj = node;
		mPos1 = initialpos;
	}
	virtual ~OgreMotionState(){}

	void setNode(Ogre::SceneNode* node)
	{
		mVisibleobj = node;
	}

	void updateTransform(btTransform& newpos)
	{
		mPos1 = newpos;
	}

	void setGameObject(GameObject* g)
	{
		gObj = g;
	}

	GameObject* getGameObject()
	{
		return gObj;
	}

	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		worldTrans = mPos1;
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		if(NULL == mVisibleobj)
			return;
		btQuaternion rot = worldTrans.getRotation();
		mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
		btVector3 pos = worldTrans.getOrigin();
		// Bullet -> Ogre
		mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
	}
};

#endif // #ifndef __OgreMotionState_h_