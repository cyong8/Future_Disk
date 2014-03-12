#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	Ogre::Real dimension = 15.0f;
	Ogre::Plane plane;

	/* Plane for Floor */
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = -dimension;
	Ogre::MeshManager::getSingleton().createPlane("Floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	(new Wall("Floor", mSceneMgr, game_simulator, plane, Ogre::Vector3::UNIT_Y))->addToSimulator();

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	Ogre::MeshManager::getSingleton().createPlane("Ceiling", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

	(new Wall("Ceiling", mSceneMgr, game_simulator, plane, Ogre::Vector3::NEGATIVE_UNIT_Y))->addToSimulator();

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	Ogre::MeshManager::getSingleton().createPlane("LeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("LeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3::UNIT_X))->addToSimulator();

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	Ogre::MeshManager::getSingleton().createPlane("RightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("RightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3::NEGATIVE_UNIT_X))->addToSimulator();
	
	/* Plane for BackWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	Ogre::MeshManager::getSingleton().createPlane("BackWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("BackWall", mSceneMgr, game_simulator, plane, Ogre::Vector3::UNIT_Z))->addToSimulator();
	
	/* Plane for FrontWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	Ogre::MeshManager::getSingleton().createPlane("FrontWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontWall", mSceneMgr, game_simulator, plane, Ogre::Vector3::NEGATIVE_UNIT_Z))->addToSimulator();

	/* Plane for BackLeftWall */
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, 1.0f);
	Ogre::MeshManager::getSingleton().createPlane("BackLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("BackLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, 1.0f)))->addToSimulator();
	
	/* Plane for BackRightWall */
	plane.normal = Ogre::Vector3(1.0f, 0.0f, 1.0f);
	Ogre::MeshManager::getSingleton().createPlane("BackRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("BackRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, 1.0f)))->addToSimulator();
	
	/* Plane for FrontLeftWall */
	plane.normal = Ogre::Vector3(1.0f, 0.0f, -1.0f);
	Ogre::MeshManager::getSingleton().createPlane("FrontLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, -1.0f)))->addToSimulator();
	
	/* Plane for FrontRightWall */
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, -1.0f);
	Ogre::MeshManager::getSingleton().createPlane("FrontRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FrontRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, -1.0f)))->addToSimulator();
}