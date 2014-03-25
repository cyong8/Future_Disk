#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident)
{
	width = 30.0f;
	height = 60.0f;
	gapSize = 10.0f; 
	floorLength =35.0f;
	//floorLength = (height + width*2/3 - gapSize) / 2.0f;
	Ogre::Real heightScalingFactor = 3.0f/4.0f;
	Ogre::Vector3 position;
	Ogre::Plane plane;
	plane.d = 0;
	Ogre::Real tileSize = 5.0f;
	Ogre::Real tileNum = (floorLength * width)/tileSize;

	// X position of the first tile for player1 is -width/2 + tileSize/2, y is 0 and z is gapsize/2 + floorLength - tileSize/2
	position = Ogre::Vector3(-width/2 + tileSize/2, -(width*heightScalingFactor)/2.0f, -(gapSize/2.0 + floorLength - tileSize/2.0));//Ogre::Vector3((-width/2.0) + tileSize/2.0, 0.0, gapSize/2.0 + floorLength - tileSize/2.0);

	/* Instead of a wall the floor is a collection of tiles */
	for(int j = 0; j < 3; j++)
	{
		for(int i = 0; i < 5; i++)
		{
			/*if(position.x >= 15.0f)
			{
				// reset x position and increment z position
				position.x = (-width/2.0f) + tileSize/2.0f;
				position.z += tileSize;//gapSize/2.0f + floorLength - tileSize/2 + tileSize;
			}*/
			Ogre::String tileName = "tile" + i;
			(new Tile(tileName, mSceneMgr, game_simulator, position, tileSize))->addToSimulator();
			// Add tile to simulator
			//tile1->addToSimulator();
			// increment x position by tileSize
			position.x += tileSize;
		}
		position.z += tileSize;
	}

	/* Plane for 2nd player Floor */
	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -(width*heightScalingFactor)/2.0f, height/2.0f + width/3.0f - floorLength/2.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, floorLength, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);
	if (ident == 0)
		(new Wall("Floor", "Floor_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength)))->addToSimulator();
	else 
		new Wall("Floor", "Floor_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength));


	/* Plane for Floor 2 */

	plane.normal = Ogre::Vector3::UNIT_Y;
	position = Ogre::Vector3(0.0f, -(width*heightScalingFactor)/2.0f, height/2.0f + width/3.0f - floorLength/2.0f);
	Ogre::MeshManager::getSingleton().createPlane("Floor2_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, floorLength, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Z);

	if (ident == 0)
		(new Wall("Floor2", "Floor2_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength)))->addToSimulator();
	else 
		new Wall("Floor2", "Floor2_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Y, position, Ogre::Vector3(width, 0.01f, floorLength));

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, (width*heightScalingFactor)/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height + width, 20, 20, true, 1, width/4.0f, height/4.0f, Ogre::Vector3::UNIT_Z);

	if (ident == 0)
		(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width)))->addToSimulator();
	else 
		new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width));

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();
	else
		new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height));

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)))->addToSimulator();
	else 
		new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height));

// Front and Back Divisions
	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2.0f - width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("FarWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f));

	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2.0f + width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("NearWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, width/4, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else
		new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f));

	Ogre::Real fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));
	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("FarLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));

	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, -height/2.0f - width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("FarRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));
	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more left)
	Ogre::MeshManager::getSingleton().createPlane("NearLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
	
	if (ident == 0)
		(new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else 
		new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));

	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, height/2.0f + width/6.0f); // change the x value (more right)
	Ogre::MeshManager::getSingleton().createPlane("NearRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	if (ident == 0)
		(new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)))->addToSimulator();
	else
		new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f));
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getWidth()
{
	return width;
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getHeight()
{
	return height;
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getFloorLength()
{
	return floorLength;
}
//-------------------------------------------------------------------------------------
