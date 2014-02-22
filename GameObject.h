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

public:
	btRigidBody* getBody(void);
	GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
	void updateTransform();
	void addToSimulator();	
	Ogre::String getGameObjectName();
	Ogre::SceneNode* getSceneNode();
	string typeName;
	btTransform tr;
};

#endif // #ifndef __GameObject_h_
