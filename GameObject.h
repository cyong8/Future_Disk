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
	btTransform tr;
	btVector3 inertia;

	btScalar mass;
	btRigidBody *body;
	bool kinematic;
	bool needsUpdates;

	/* Use this later when we define the struct that contains 
       all the collision info for a given GameObject
       (See Code snippets from 2/13 on class website)
	*/
	//CollisionContext* context;
	

public:
	btRigidBody* getBody(void);
	GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim);
	void updateTransform();
	void addToSimulator();	

	double worldScale;
};

#endif // #ifndef __GameObject_h_
