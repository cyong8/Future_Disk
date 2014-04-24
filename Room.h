#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
#include "Tile.h"

enum wallIndex
{
	Ceiling, 
	LeftWall, 
	RightWall, 
	FarWall, 
	NearWall, 
	FarLeftWall, 
	FarRightWall, 
	NearLeftWall, 
	NearRightWall
};

class Wall;

class Tile;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident, int numberOfPlayers);
	Ogre::Real getWidth(void);
	Ogre::Real getHeight(void);
	Ogre::Real getGapSize(void);
	Ogre::Real getFloorPositionY(void);
	Ogre::SceneNode* getClientGapSceneNode(void);
	Ogre::SceneNode* getHostGapSceneNode(void);
	Wall* getWall(int index);

	vector<Tile*> cTileList;
	vector<Tile*> hTileList;
	vector<Wall*> wallList;
protected:
	Ogre::SceneNode* clientGapNode;
	Ogre::SceneNode* hostGapNode;
	Ogre::Real width;
	Ogre::Real height;
	Ogre::Real heightScalingFactor;
	
	Ogre::Real gapSize;
	Ogre::Real tileSize;
	Ogre::Real tilesPerRow;
	Ogre::Real tilesPerCol;
	Ogre::Real tilesPerPlayer;
	
	Ogre::Real floorPositionY;
	int numberOfPlayers;
};

#endif // #ifndef __Room_h_
