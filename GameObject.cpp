#include "GameObject.h"
#include "Simulator.h"
#include "OgreMotionState.h"

GameObject::GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim)
{
	name = nym;
	sceneMgr = mgr;
	simulator = sim;
	rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	shape = NULL;
	motionState = NULL;
	tr.setIdentity();
	mass = 0.0f;
	kinematic = false;
	needsUpdates = false;
	inertia.setZero(); // resistence to a change in direction; YEAH, I forgot!
}

btRigidBody* GameObject::getBody()	
{
	return (*this).body;		
}

void GameObject::updateTransform() 
{
	Ogre::Vector3 pos = rootNode->getPosition();
	tr.setOrigin(btVector3(pos.x,pos.y,pos.z));
	Ogre::Quaternion qt = rootNode->getOrientation();
	tr.setRotation(btQuaternion(qt.x,qt.y,qt.z,qt.w));
	if(motionState) 
		motionState->updateTransform(tr);	
}

Ogre::String GameObject::getGameObjectName()
{
	return this->name;
}

void GameObject::addToSimulator()
{
	motionState = new OgreMotionState(tr, rootNode);
	motionState->setGameObject(this);
	updateTransform();
	
	if(mass != 0.0f || typeName != "Disk" || typeName == "Wall") 
		shape->calculateLocalInertia(mass, inertia);


	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
	
	if(typeName == "Disk")
	{
		rbInfo.m_friction = 0.0f;
	}

	body = new btRigidBody(rbInfo);
	simulator->addObject(this);
}
