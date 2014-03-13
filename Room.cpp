#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	Ogre::Real width = 20.0f;
	Ogre::Real height = 60.0f;
	Ogre::Vector3 position;
	Ogre::Plane plane;
	plane.d = 0;

	/* Plane for Floor */
	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -width/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);

	(new Wall("Floor", "Floor_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, height)))->addToSimulator();

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, width/2, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height, 20, 20, true, 1, width/4.0f, height/4.0f, Ogre::Vector3::UNIT_Z);

	(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height)))->addToSimulator();

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width, height)))->addToSimulator();

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width, height)))->addToSimulator();
	
	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2);
	Ogre::MeshManager::getSingleton().createPlane("FarWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, width, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	(new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width, width, 0.01f)))->addToSimulator();
	
	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2);
	Ogre::MeshManager::getSingleton().createPlane("NearWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, width, 20, 20, true, 1, width/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width, width, 0.01f)))->addToSimulator();

/*	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, 1.0f);
	position = ;
	Ogre::MeshManager::getSingleton().createPlane("FarLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FarLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, 1.0f), dimension));//->addToSimulator();
	
	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3(1.0f, 0.0f, 1.0f);
	position = ;
	Ogre::MeshManager::getSingleton().createPlane("FarRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FarRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, 1.0f), dimension));//->addToSimulator();
	
	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3(1.0f, 0.0f, -1.0f);
	position = ;
	Ogre::MeshManager::getSingleton().createPlane("NearLeftWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("NearLeftWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(1.0f, 0.0f, -1.0f), dimension));//->addToSimulator();
	
	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3(-1.0f, 0.0f, -1.0f);
	position = ;
	Ogre::MeshManager::getSingleton().createPlane("NearRightWall", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	dimension/2, dimension, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("NearRightWall", mSceneMgr, game_simulator, plane, Ogre::Vector3(-1.0f, 0.0f, -1.0f), dimension));//->addToSimulator();
*/
}