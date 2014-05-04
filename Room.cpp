#include "Room.h"
#include <vector>
#include "Simulator.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *gameSimulator, int numPlayers)
{
	roomSimulator = gameSimulator;
	active = false;
	rSceneMgr = mSceneMgr;
	
	char tileNameBuffer[100];
	char hTileNameBuffer[100];
	char gapBufferNode[35];
	char gapBufferPlane[35];
	char gapBufferEntity[35];
	
	int roomID = 2;
	if (numPlayers > 2)
		roomID = 4;
	

	if (gameSimulator != NULL)
		gameSimulator->setGameRoom(this);

	playerRoomSpaces = vector<RoomSpace*>(numPlayers, NULL);

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
		heightScalingFactor *= 2.0f/3.0f; /* Will fuck up the room */
		fnSideWidth = Ogre::Math::Sqrt(Ogre::Math::Sqr(width/3) + Ogre::Math::Sqr(width/3));
	}

	/* Tile Attributes */
	tileSize = 5.0f;
	tilesPerRow = width/tileSize;
	tilesPerCol = ((height/2.0f) + (width/3.0f) - (gapSize/2.0f))/tileSize;
	if (numberOfPlayers > 2)
		tilesPerRow = ((width/2.0f) - (gapSize/2.0f))/tileSize;
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
	
	Ogre::Vector3 tilePosition = Ogre::Vector3(tilePosX, tilePosY, tilePosZ);
	Ogre::Vector3 nTilePosition = Ogre::Vector3(tilePosX, tilePosY, nTilePosZ);

	floorPositionY = tilePosY;

	/* Create Gap Planes for each Player */
	Ogre::SceneNode* clientGapNode;
	
	Ogre::Real hGapX = 0.0f;
	Ogre::Real hGapZ = gapSize/2.0f;

	Ogre::Real vGapX = gapSize/2.0f;
	Ogre::Real vGapZ = height/2.0f + width/3.0f + gapSize/2.0f;

	Ogre::Vector3 hGapPosition;
	Ogre::Vector3 vGapPosition;
	Ogre::Entity* gapEnt;

	Ogre::Real hPlaneWidth = width;
	Ogre::Real vPlaneWidth;

	if (numberOfPlayers > 2)
	{
		Ogre::Real vPlaneWidth = (height/2.0f + width/3.0f - gapSize/2.0f);
		
		hGapX = (width/2.0f + gapSize/2.0f)/2.0f;
		hPlaneWidth = (width/2.0f - gapSize/2.0f);
	}

	for (int player = 1; player <= numberOfPlayers; player++)
	{
		RoomSpace *gp = new RoomSpace;
		gp->active = false;

		if (player == 1 || player == 2)
		{
			hGapPosition = Ogre::Vector3(-hGapX, 0.0f, hGapZ);
			vGapPosition = Ogre::Vector3(-vGapX, 0.0f, vGapZ);
		}
		else
		{
			hGapPosition = Ogre::Vector3(hGapX, 0.0f, hGapZ);
			vGapPosition = Ogre::Vector3(vGapX, 0.0f, vGapZ);
		}
		if (player == 2 || player == 4)
		{
			hGapPosition = Ogre::Vector3(hGapPosition.x, 0.0f, -hGapZ);
			vGapPosition = Ogre::Vector3(vGapPosition.x, 0.0f, -vGapZ);
		}

			/************** Horizontal Gap **************/
		sprintf(gapBufferNode, "%d_HGap_%d", roomID, player);
		sprintf(gapBufferPlane, "%s%s", gapBufferNode, "_Plane");
		sprintf(gapBufferEntity, "%s%s", gapBufferNode, "_Entity");
	
		if (player == 2 || player == 4)
			plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
		else
			plane.normal = Ogre::Vector3::UNIT_Z;
		
		Ogre::MeshManager::getSingleton().createPlane(gapBufferPlane, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
		hPlaneWidth, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);
		
		clientGapNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(gapBufferNode);
		gapEnt = mSceneMgr->createEntity(gapBufferEntity, gapBufferPlane);
		clientGapNode->attachObject(gapEnt);
		gapEnt->setMaterialName("Examples/SpaceSkyPlane");
		gapEnt->setCastShadows(false);
		clientGapNode->setPosition(hGapPosition);
		clientGapNode->setVisible(false);
		gp->horizontalGap = clientGapNode;

		if (numberOfPlayers > 2)
		{
				/************** Vertical Gap **************/
			sprintf(gapBufferNode, "%d_VGap_%d", roomID, player);
			sprintf(gapBufferPlane, "%s%s", gapBufferNode, "_Plane");
			sprintf(gapBufferEntity, "%s%s", gapBufferNode, "_Entity");

			if (player == 1 || player == 3)
				plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
			else
				plane.normal = Ogre::Vector3::UNIT_X;

			Ogre::MeshManager::getSingleton().createPlane(gapBufferPlane, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
			vPlaneWidth, width*heightScalingFactor, 20, 20, true, 1, width/4, height/4, Ogre::Vector3::UNIT_Y);
			
			clientGapNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(gapBufferNode);
			gapEnt = mSceneMgr->createEntity(gapBufferEntity, gapBufferPlane);
			clientGapNode->attachObject(gapEnt);
			gapEnt->setMaterialName("Examples/SpaceSkyPlane");
			gapEnt->setCastShadows(false);
			clientGapNode->setPosition(vGapPosition);
			clientGapNode->setVisible(false);
			gp->verticalGap = clientGapNode;
		}

		playerRoomSpaces[player - 1] = gp;
	}

	Tile* localTile;
	Ogre::Vector3 localPosition;
	/***************** TILES OF FLOOR *****************/
	for (int row = 0; row < tilesPerCol; row++)	// change the Z
	{
		for (int col = 0; col < tilesPerRow; col++) // change the X
		{	
			for (int player = 1; player <= numberOfPlayers; player++)
			{
				if (player == 2 || player == 4)
					localPosition = nTilePosition;
				else
					localPosition = tilePosition;

				if (player == 3 || player == 4)
					localPosition = Ogre::Vector3(localPosition.x + (width/2.0f + gapSize/2.0f), tilePosY, localPosition.z);

				sprintf(tileNameBuffer, "%d_client%d_%d%d", roomID, player, col, row);
					
				localTile = new Tile(tileNameBuffer, mSceneMgr, gameSimulator, localPosition, tileSize, (row*tilesPerRow + col), player);
				playerRoomSpaces[player - 1]->tileList.push_back(localTile);
			}
			tilePosition = Ogre::Vector3(tilePosition.x + tileSize, tilePosY, tilePosition.z);
			nTilePosition = Ogre::Vector3(nTilePosition.x + tileSize, tilePosY, nTilePosition.z);
		}
		tilePosition = Ogre::Vector3(tilePosX, tilePosY, tilePosition.z - tileSize);
		nTilePosition = Ogre::Vector3(tilePosX, tilePosY, nTilePosition.z + tileSize);
	}

	char wallCBuffer[100];
	char wallCPlaneBuffer[100];

	sprintf(wallCBuffer, "%d%s", roomID, "_Ceiling");
	sprintf(wallCPlaneBuffer, "%d%s", roomID, "__Ceiling_Plane");

	/* Plane for Ceiling */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Y;
	position = Ogre::Vector3(0.0f, (width*heightScalingFactor)/2.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallCPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width, height + width, 20, 20, true, 1, width/4, (height*heightScalingFactor)/4, Ogre::Vector3::UNIT_Z);

	wallList.push_back(new Wall(wallCBuffer, wallCPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3::NEGATIVE_UNIT_Y, position, Ogre::Vector3(width, 0.01f, height + width)));

	char wallLBuffer[100];
	char wallLPlaneBuffer[100];

	sprintf(wallLBuffer, "%d%s", roomID, "_LeftWall");
	sprintf(wallLPlaneBuffer, "%d%s", roomID, "_LeftWall_Plane");

	/* Plane for LeftWall */
	plane.normal = Ogre::Vector3::UNIT_X;
	position = Ogre::Vector3(-width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallLPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallLBuffer, wallLPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3::UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)));

	char wallRBuffer[100];
	char wallRPlaneBuffer[100];

	sprintf(wallRBuffer, "%d%s", roomID, "_RightWall");
	sprintf(wallRPlaneBuffer, "%d%s", roomID, "_RightWall_Plane");

	/* Plane for RightWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_X;
	position = Ogre::Vector3(width/2.0f, 0.0f, 0.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallRPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	height, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallRBuffer, wallRPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3::NEGATIVE_UNIT_X, position, Ogre::Vector3(0.01f, width*heightScalingFactor, height)));
	
// Front and Back Divisions
	char wallFBuffer[100];
	char wallFPlaneBuffer[100];

	sprintf(wallFBuffer, "%d%s", roomID, "_FarWall");
	sprintf(wallFPlaneBuffer, "%d%s", roomID, "_FarWall_Plane");

	/* Plane for FarWall */
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, -height/2.0f - width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallFPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallFBuffer, wallFPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3::UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)));

	char wallNBuffer[100];
	char wallNPlaneBuffer[100];

	sprintf(wallNBuffer, "%d%s", roomID, "_NearWall");
	sprintf(wallNPlaneBuffer, "%d%s", roomID, "_NearWall_Plane");

	/* Plane for NearWall */
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(0.0f, 0.0f, height/2.0f + width/3.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallNPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	width/3, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallNBuffer, wallNPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3::NEGATIVE_UNIT_Z, position, Ogre::Vector3(width/3, width*heightScalingFactor, 0.01f)));

	char wallFLBuffer[100];
	char wallFLPlaneBuffer[100];

	sprintf(wallFLBuffer, "%d%s", roomID, "_FarLeftWall");
	sprintf(wallFLPlaneBuffer, "%d%s", roomID, "_FarLeftWall_Plane");

	// Plane for FarLeftWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, -height/2.0f - width/6.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallFLPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallFLBuffer, wallFLPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3(-1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	char wallFRBuffer[100];
	char wallFRPlaneBuffer[100];

	sprintf(wallFRBuffer, "%d%s", roomID, "_FarRightWall");
	sprintf(wallFRPlaneBuffer, "%d%s", roomID, "_FarRightWall_Plane");

	// Plane for FarRightWall 
	plane.normal = Ogre::Vector3::UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, -height/2.0f - width/6.0f); 
	Ogre::MeshManager::getSingleton().createPlane(wallFRPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallFRBuffer, wallFRPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	char wallNLBuffer[100];
	char wallNLPlaneBuffer[100];

	sprintf(wallNLBuffer, "%d%s", roomID, "_NearLeftWall");
	sprintf(wallNLPlaneBuffer, "%d%s", roomID, "_NearLeftWall_Plane");

	// Plane for NearLeftWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(-width/3.0f, 0.0f, height/2.0f + width/6.0f); 
	Ogre::MeshManager::getSingleton().createPlane(wallNLPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);
	
	wallList.push_back(new Wall(wallNLBuffer, wallNLPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3(1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	char wallNRBuffer[100];
	char wallNRPlaneBuffer[100];

	sprintf(wallNRBuffer, "%d%s", roomID, "_NearRightWall");
	sprintf(wallNRPlaneBuffer, "%d%s", roomID, "_NearRightWall_Plane");

	// Plane for NearRightWall 
	plane.normal = Ogre::Vector3::NEGATIVE_UNIT_Z;
	position = Ogre::Vector3(width/3.0f, 0.0f, height/2.0f + width/6.0f);
	Ogre::MeshManager::getSingleton().createPlane(wallNRPlaneBuffer, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
	fnSideWidth, width*heightScalingFactor, 20, 20, true, 1, (height*heightScalingFactor)/4, width/4, Ogre::Vector3::UNIT_Y);

	wallList.push_back(new Wall(wallNRBuffer, wallNRPlaneBuffer, mSceneMgr, gameSimulator, Ogre::Vector3(-1.0f, 0.0f, -1.0f), position, Ogre::Vector3(fnSideWidth, width*heightScalingFactor, 0.01f)));

	for (int i = 0; i < wallList.size(); i++)
		wallList[i]->getSceneNode()->setVisible(false);
}
//-------------------------------------------------------------------------------------
RoomSpace* Room::getPlayerRoomSpace(int playerID)
{
	return playerRoomSpaces[playerID-1];
}
//-------------------------------------------------------------------------------------
int Room::getNumberOfPlayers()
{
	if (numberOfPlayers == 3)
		return numberOfPlayers + 1;
	else 
		return numberOfPlayers;
}
//-------------------------------------------------------------------------------------
Wall* Room::getWall(int index)
{
	return wallList[index];
}
//-------------------------------------------------------------------------------------
void Room::activateRoom()
{
	// Add all player tiles to the simulator and/or make visible
	for (int i = 0; i < playerRoomSpaces.size(); i++)
	{
		RoomSpace* localSpace = playerRoomSpaces[i];

		if (!localSpace->active)
		{
			for (int j = 0; j < localSpace->tileList.size(); j++)
			{
				if (roomSimulator != NULL)
					localSpace->tileList[j]->addToSimulator();
				localSpace->tileList[j]->getSceneNode()->setVisible(true);
			}	
			localSpace->active = true;
		}
	}
	// Add all walls to the simulator and/or make visible
	for (int i = 0; i < wallList.size(); i++)
	{
		if (roomSimulator != NULL)
			wallList[i]->addToSimulator();
		wallList[i]->getSceneNode()->setVisible(true);
	}

	rSceneMgr->getLight("roomLight")->setPosition(Ogre::Vector3(0.0f, -floorPositionY, 0.0f));
	active = true;
}
//-------------------------------------------------------------------------------------
void Room::deactivateRoom()
{
	// Add all player tiles to the simulator and/or make visible
	for (int i = 0; i < playerRoomSpaces.size(); i++)
	{
		RoomSpace* localSpace = playerRoomSpaces[i];

		if (localSpace->active)
		{			
			for (int j = 0; j < localSpace->tileList.size(); j++)
			{
				if (roomSimulator != NULL)
					roomSimulator->removeObject(localSpace->tileList[j]->getGameObjectName());
				localSpace->tileList[j]->getSceneNode()->setVisible(false);
			}
			localSpace->active = false;
		}
	}
	// Add all walls to the simulator and/or make visible
	for (int i = 0; i < wallList.size(); i++)
	{
		if (roomSimulator != NULL)
			roomSimulator->removeObject(wallList[i]->getGameObjectName());
		wallList[i]->getSceneNode()->setVisible(false);
	}
	active = false;
}
//-------------------------------------------------------------------------------------
void Room::deactivateRoomSpace(int playerID)
{
	RoomSpace* localSpace = playerRoomSpaces[playerID - 1];
	int localNumTiles = localSpace->tileList.size();

	if (localSpace->active)
	{
		for (int i = 0; i < localNumTiles; i++)
		{
			if (roomSimulator != NULL)
				roomSimulator->removeObject(localSpace->tileList[i]->getGameObjectName());
			localSpace->tileList[i]->getSceneNode()->setVisible(false);
		}
		localSpace->active = false;
	}
}
//-------------------------------------------------------------------------------------