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
struct RoomSpace
{
	Ogre::SceneNode* horizontalGap;
	Ogre::SceneNode* verticalGap;

	vector<Tile*> tileList;
};

class Wall;
class Tile;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int numberOfPlayers);
	Ogre::Real getWidth(void) { return width; } ;
	Ogre::Real getHeight(void) { return height; } ;
	Ogre::Real getGapSize(void) { return gapSize; } ;
	Ogre::Real getFloorPositionY(void) { return floorPositionY; } ;
	RoomSpace* getPlayerRoomSpace(int playerID);
	Wall* getWall(int index);
	int getNumberOfPlayers(void);
	Ogre::Vector3 getBounds(void);
	int getTilesPerRow() { return (int)tilesPerRow; };
	int getTilesPerColumn() { return (int)tilesPerCol; };

	vector<Tile*> cTileList;
	vector<Tile*> hTileList;

	vector<Wall*> wallList;
protected:
	vector<RoomSpace*> playerRoomSpaces;
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
