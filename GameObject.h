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
	Simulator *simulator;
	Ogre::SceneNode *rootNode;
	Ogre::Entity *geom;
	btCollisionShape *shape;
	btScalar mass;
	btRigidBody *body;
	btTransform tr;
	btVector3 inertia;
	OgreMotionState *motionState;

public:
	btRigidBody* getBody(void);
	GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
	void updateTransform();
	void addToSimulator();	

};

#endif // #ifndef __GameObject_h_