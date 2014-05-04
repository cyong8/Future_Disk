#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
#include "Tile.h"
#include "BaseApplication.h"

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
	bool active;
};

class Wall;
class Tile;
class Simulator;

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
	void activateRoom(void);
	void deactivateRoom(void);
	void deactivateRoomSpace(int playerID);
	int getTilesPerRow(void) { return (int)tilesPerRow; };
	int getTilesPerColumn(void) { return (int)tilesPerCol; };
	bool checkActive(void) { return active; };

	vector<Tile*> cTileList;
	vector<Tile*> hTileList;

	vector<Wall*> wallList;
protected:
	Simulator* roomSimulator;
	Ogre::SceneManager* rSceneMgr;

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
	bool active;
};

#endif // #ifndef __Room_h_
