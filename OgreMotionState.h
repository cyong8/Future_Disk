#ifndef __OgreMotionState_h_
#define __OgreMotionState_h_

class OgreMotionState : public btMotionState
{
protected:
	Ogre::SceneNode* mVisibelobj;
	btTransform mPos1;
public:
	OgreMotionState(const btTransform& initialpos, Ogre::SceneNode* node)
	{
		mVisibleobj = node;
		mPos1 = initalpos;
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

	virtual void getWorldTransform(btTransform& worldTrans) const
	{
		worldTrans = mPos1;
	}

	virtual void setWorldTransform(const btTransform& worldTrans)
	{
		if(NULL == mVisibleobj)
			return; // 
		btQuaternion rot = worldTrans.getRotation();
		mVisisbleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
		btVector3 pos = worldTrans.getOrigin();
		mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());

	}
};

#endif // #ifndef __OgreMotionState_h_