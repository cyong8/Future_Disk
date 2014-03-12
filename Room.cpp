#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	Ogre::Real dimension = 15.0f;
	Ogre::Plane plane;

	/* Plane for Floor */
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("Floor_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Z);

	(new Wall("Floor", "Floor_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, dimension))->addToSimulator();

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Z);

	(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, dimension));//->addToSimulator();

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Y);

	(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, dimension));//->addToSimulator();

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Y);

	(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, dimension));//->addToSimulator();
	
	/* Plane for BackWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("BackWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Y);

	(new Wall("BackWall", "BackWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, dimension));//->addToSimulator();
	
	/* Plane for FrontWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	plane.d = dimension/2;
	Ogre::MeshManager::getSingleton().createPlane("FrontWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension, dimension, 20, 20, true, 1, dimension/4, dimension/4, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontWall", "FrontWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, dimension));//->addToSimulator();

/*	// Plane for BackLeftWall 
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, 1.0f);
	Ogre::MeshManager::getSingleton().createPlane("BackLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("BackLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, 1.0f), dimension));//->addToSimulator();
	
	// Plane for BackRightWall 
	plane.normal = Ogre::Vector3(1.0f, 0.0f, 1.0f);
	Ogre::MeshManager::getSingleton().createPlane("BackRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("BackRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, 1.0f), dimension));//->addToSimulator();
	
	// Plane for FrontLeftWall 
	plane.normal = Ogre::Vector3(1.0f, 0.0f, -1.0f);
	Ogre::MeshManager::getSingleton().createPlane("FrontLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, -1.0f), dimension));//->addToSimulator();
	
	// Plane for FrontRightWall 
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, -1.0f);
	Ogre::MeshManager::getSingleton().createPlane("FrontRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, -1.0f), dimension));//->addToSimulator();
*/
}