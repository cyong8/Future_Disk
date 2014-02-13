



class GameObject {
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

    GameObject::GameObject(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim)
    {

	name = nym;
	sceneMgr = mgr;
	siumlator = sim;
	rootNode = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
	shape = NULL;
	tr.setIdentity();
	mass = 0.0f;
	inertia.setZero();

    }
    void GameObject::updateTransform() 
    {

	Ogre::Vector3 pos = rootNode->getPosition();
	tr.setOrigin(btVector3(pos.x,pos.y,pos.z));
	Ogre::Quaternion qt = rootNode->getOrientation();
	tr.setRotation(btQuaternion(qt.x,qt.y,qt.z,qt.w));
	if(motionState) motionState->updateTransform(tr);	

    }
    void GameObject::addToSimulator()
    {
	updateTransform();
	motionState = ne OgreMotionState(tr, rootNode);
	if(mass != 0.0f) shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
	body = new btRigidBody(rbInfo);
	simulator->addObject(this);
    }
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        BasicTut2 app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
