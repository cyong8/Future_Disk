#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	Ogre::Real width = 30.0f;
	Ogre::Real height = 60.0f;
	Ogre::Real floorLength = 35.0f;
	Ogre::Real heightScalingFactor = 3.0f/4.0f;
	Ogre::Vector3 position;
	Ogre::Plane plane;
	plane.d = 0;

	/* Plane for Floor 1 */
	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -(width*heightScalingFactor)/2.0f, -height/2.0f - width/3.0f + floorLength/2.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, floorLength, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);

	(new Wall("Floor", "Floor_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength)))->addToSimulator();
	
	/* Plane for Floor 2 */
	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -(width*heightScalingFactor)/2.0f, height/2.0f + width/3.0f - floorLength/2.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor2_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, floorLength, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);

	(new Wall("Floor2", "Floor2_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength)))->addToSimulator();

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, (width*heightScalingFactor)/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height + width, 20, 20, true, 1, width/4.0f, height/4.0f, Ogre::Vector3::UNIT_Z);

	(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width)))->addToSimulator();

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();
	
// Front and Back Divisions
	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2.0f - width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("FarWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	(new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();
	
	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2.0f + width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("NearWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, width/4, Ogre::Vector3::UNIT_Y);

	(new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();

	Ogre::Real fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));
	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("FarLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();

	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("FarRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	
	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("NearLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	
	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("NearRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	(new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
}
