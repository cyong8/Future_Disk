#ifndef __GameObject_h_
#define __GameObject_h_

#include "BaseApplication.h"

class Simulator;
class OgreMotionState;

class GameObject 
{
protected:
	Ogre::String name;
	Ogre::SceneManager* sceneMgr;
	Ogre::SceneNode *rootNode;
	Ogre::Entity *geom;
	OgreMotionState *motionState;

	Simulator *simulator;
	btCollisionShape *shape;
	btVector3 inertia;

	btScalar mass;
	btRigidBody *body;
	bool kinematic;
	bool needsUpdates;
	bool hit;
	bool ReAddFlag;

public:
	GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
	btRigidBody* getBody(void);
	void updateTransform(void);
	void addToSimulator(void);	
	void removeFromSimulator(void);
	Ogre::String getGameObjectName(void);
	Ogre::SceneNode* getSceneNode(void);
	OgreMotionState* getOgreMotionState(void);
	bool checkReAddFlag(void);
	bool checkHitFlag(void);
	void toggleHitFlag(void);
	string typeName;
	btTransform tr;
};

#endif // #ifndef __GameObject_h_
