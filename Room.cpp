#include "Room.h"
#include <vector>

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident, int numPlayers)
{
	char tileNameBuffer[100];
	char hTileNameBuffer[100];

	numberOfPlayers = numPlayers;

	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", false);
	
	/* Room Dimensions */
	width = 30.0f;			// along x-axis 
	height = 60.0f;			// along z-axis
	heightScalingFactor = 3.0f/4.0f;
	gapSize = 10.0f;		/* NOTE: gapSize, width, and height must be divisible by the tileSize */	
	Ogre::Real fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));

	if (numberOfPlayers > 2)
	{
		width *= 2;
		height *= 2;
		// heightScalingFactor *= 2;
		fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));
	}

	/* Tile Attributes */
	tileSize = 5.0f;
	tilesPerRow = (((height/2.0f) + (width/3.0f)) - (gapSize/2.0f))/tileSize;
	tilesPerCol = width/tileSize;
	if (numberOfPlayers > 2)
		tilesPerCol = ((width/2.0f) + (gapSize/2.0f))/tileSize;
	tilesPerPlayer = tilesPerRow * tilesPerCol;

	/* Plane Creation */
	Ogre::Plane plane;
	plane.d = 0;
	Ogre::Vector3 position;

	/* Tile Placement */
	Ogre::Real tilePosX = -width/2.0f + tileSize/2.0f;
	Ogre::Real tilePosY = -(width*heightScalingFactor)/2.0f; 
	Ogre::Real tilePosZ = height/2.0f + width/3.0f - tileSize/2.0f;
	Ogre::Real nTilePosZ = (-height/2.0f - width/3.0f) + tileSize/2.0f;
	printf("Tile positions: Vector3(%f, %f, %f)\n", tilePosX, tilePosY, tilePosZ);
	printf("nTile positions: Vector3(%f, %f, %f)\n", tilePosX, tilePosY, nTilePosZ);
	
	Ogre::Vector3 tilePosition = Ogre::Vector3(tilePosX, tilePosY, tilePosZ);
	Ogre::Vector3 nTilePosition = Ogre::Vector3(tilePosX, tilePosY, nTilePosZ);

	// Ogre::Real tilePosX = -width/2.0f + tileSize/2.0f;
	// Ogre::Real tilePosY = -(width*heightScalingFactor)/2.0f;
	// Ogre::Real hTilePosZ = height/2.0f + width/3.0f - tileSize/2.0f;
	// Ogre::Real cTilePosZ = (-height/2.0f - width/3.0f) + tileSize/2.0f;
	// Ogre::Vector3 hTilePosition = Ogre::Vector3(tilePosX, tilePosY, hTilePosZ);
	// Ogre::Vector3 cTilePosition = Ogre::Vector3(tilePosX, tilePosY, cTilePosZ);

	floorPositionY = tilePosY;
	int arrayIndex = 0;	

	/* Gap Box to restrict Player movement */ // use _getWorldAABB
		// Client Gap
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -(gapSize/2.0f));
	Ogre::MeshManager::getSingleton().createPlane("ClientGapPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);
	
	clientGapNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("ClientGap");
	Ogre::Entity* cgEnt = mSceneMgr->createEntity("ClientGapEntity", "ClientGapPlane");
	clientGapNode->attachObject(cgEnt);
	cgEnt->setMaterialName("Examples/SpaceSkyPlane");
	cgEnt->setCastShadows(false);
	clientGapNode->setPosition(position);
	clientGapNode->setVisible(false);
	
		// Host Gap
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, (gapSize/2.0f));
	Ogre::MeshManager::getSingleton().createPlane("HostGapPlane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	hostGapNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HostGap");
	Ogre::Entity* hgEnt = mSceneMgr->createEntity("HostGapEntity", "HostGapPlane");
	hostGapNode->attachObject(hgEnt);
	hgEnt->setMaterialName("Examples/SpaceSkyPlane");
	hgEnt->setCastShadows(false);
	hostGapNode->setPosition(position);
	hostGapNode->setVisible(false);

	Tile* localTile;
	Ogre::Vector3 localPosition;
	/* Tiles of Floor */
	for (int row = 0; row < tilesPerRow; row++)	// change the Z
	{
		printf("******NEW ROW**********\n");
		for (int col = 0; col < tilesPerCol; col++) // change the X
		{	
			for (int player = 1; player <= numberOfPlayers; player++)
			{
				if (player > 2)
				{
					printf("Tile for player %d: nTile = Vector3(%f, %f, %f)\t", player, nTilePosition.x, nTilePosition.y, nTilePosition.z);					
					localPosition = nTilePosition;
				}
				else
				{
					printf("Tile for player %d: nTile = Vector3(%f, %f, %f)\t", player, tilePosition.x, tilePosition.y, tilePosition.z);			
					localPosition = tilePosition;
				}

				if (player == 2 || player == 4)
					localPosition = Ogre::Vector3(localPosition.x + (width/2.0f + gapSize/2.0f), tilePosY, localPosition.z);

				printf("local = Vector3(%f, %f, %f)\n", localPosition.x, localPosition.y, localPosition.z);
				sprintf(tileNameBuffer, "client%d_%d%d", player, row, col);
				
				localTile = new Tile(tileNameBuffer, mSceneMgr, game_simulator, localPosition, tileSize);

				if (ident == 0)	// if host, add to simulator
					localTile->addToSimulator();
			}
			tilePosition = Ogre::Vector3(tilePosition.x + tileSize, tilePosY, tilePosition.z);
			nTilePosition = Ogre::Vector3(nTilePosition.x + tileSize, tilePosY, nTilePosition.z);
		}
		tilePosition = Ogre::Vector3(tilePosX, tilePosY, tilePosition.z - tileSize);
		nTilePosition = Ogre::Vector3(tilePosX, tilePosY, nTilePosition.z + tileSize);
	}


	// for (int row = 0; row < tilesPerRow; row++)	// change the Z
	// {
	// 	for (int col = 0; col < tilesPerCol; col++) // change the X
	// 	{	
	// 		sprintf(cTileNameBuffer, "client%d%d", row, col, numberOfPlayers);
	// 		cTileList.push_back(new Tile(cTileNameBuffer, mSceneMgr, game_simulator, cTilePosition, tileSize));

	// 		sprintf(hTileNameBuffer, "host%d%d", row, col, numberOfPlayers);
	// 		hTileList.push_back(new Tile(hTileNameBuffer, mSceneMgr, game_simulator, hTilePosition, tileSize));

	// 		cTilePosition = Ogre::Vector3(cTilePosition.x + tileSize, tilePosY, cTilePosition.z);
	// 		hTilePosition = Ogre::Vector3(hTilePosition.x + tileSize, tilePosY, hTilePosition.z);
		
// 			if (ident == 0)	// if host, add to simulator
// 			{
// 				cTileList[arrayIndex]->addToSimulator();
// 				hTileList[arrayIndex]->addToSimulator();
// 				arrayIndex++;
// 			}
	// 	}
	// 	cTilePosition = Ogre::Vector3(tilePosX, tilePosY, cTilePosition.z + tileSize);
	// 	hTilePosition = Ogre::Vector3(tilePosX, tilePosY, hTilePosition.z - tileSize);
	// }

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, (width*heightScalingFactor)/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("Ceiling_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height + width, 20, 20, true, 1, width/4.0f, height/4.0f, Ogre::Vector3::UNIT_Z);

	wallList.push_back(new Wall("Ceiling", "Ceiling_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width)));

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("LeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("LeftWall", "LeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)));

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane("RightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, height/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("RightWall", "RightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)));
	

// Front and Back Divisions
	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2.0f - width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("FarWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("FarWall", "FarWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)));

	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2.0f + width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane("NearWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, width/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("NearWall", "NearWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)));

	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, -height/2.0f - width/6.0f);
	Ogre::MeshManager::getSingleton().createPlane("FarLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("FarLeftWall", "FarLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, -height/2.0f - width/6.0f); 
	Ogre::MeshManager::getSingleton().createPlane("FarRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("FarRightWall", "FarRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, height/2.0f + width/6.0f); 
	Ogre::MeshManager::getSingleton().createPlane("NearLeftWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
	
	wallList.push_back(new Wall("NearLeftWall", "NearLeftWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, height/2.0f + width/6.0f);
	Ogre::MeshManager::getSingleton().createPlane("NearRightWall_Plane", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall("NearRightWall", "NearRightWall_Plane", mSceneMgr, game_simulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	if (ident == 0)
	{
		for (int i = 0; i < wallList.size(); i++)
			wallList[i]->addToSimulator();	
	}
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
Ogre::Real Room::getGapSize()
{
	return gapSize;
}
//-------------------------------------------------------------------------------------
Ogre::Real Room::getFloorPositionY()
{
	return floorPositionY;
}
//-------------------------------------------------------------------------------------
Ogre::SceneNode* Room::getClientGapSceneNode()
{
	return clientGapNode;
}
//-------------------------------------------------------------------------------------
Ogre::SceneNode* Room::getHostGapSceneNode()
{
	return hostGapNode;
}
Wall* Room::getWall(int index)
{
	return wallList[index];
}
