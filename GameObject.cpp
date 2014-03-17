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
	inertia.setZero(); 
	kinematic = false;
	needsUpdates = false;
	ReAddFlag = false;
	hit = false;
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
void GameObject::addToSimulator()
{
	motionState = new OgreMotionState(tr, rootNode);
	motionState->setGameObject(this);
	updateTransform();

	if(mass != 0.0f || typeName != "Disk") 
		shape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
	
	if(typeName == "Disk")
	{
		rbInfo.m_friction = 0.0f;
	}

	if(typeName == "Player")
	{
		rbInfo.m_friction = 5.0f;
	}

	body = new btRigidBody(rbInfo);
	simulator->addObject(this);
}
void GameObject::removeFromSimulator()
{
	delete motionState;
	delete body;
	ReAddFlag = true;
}
Ogre::String GameObject::getGameObjectName()
{
	return this->name;
}
Ogre::SceneNode* GameObject::getSceneNode()
{
	return this->rootNode;
}
OgreMotionState* GameObject::getOgreMotionState()
{
	return this->motionState;
}
bool GameObject::checkReAddFlag()
{
	return this->ReAddFlag;
}