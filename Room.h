#ifndef __Room_h_
#define __Room_h_

#include "Wall.h"
#include "Tile.h"

class Wall;

class Tile;

class Room
{
public:
	Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator, int ident);
	Ogre::Vector3 getBounds(void);
	Ogre::Real getWidth(void);
	Ogre::Real getHeight(void);
	Ogre::Real getFloorLength(void);
	Ogre::Real getFloorPositionY(void);
	vector<Tile*> cTileList;
	vector<Tile*> hTileList;
protected:
	Ogre::Real width;
	Ogre::Real height;
	Ogre::Real heightScalingFactor;
	
	Ogre::Real gapSize;
	Ogre::Real tileSize;
	Ogre::Real tilesPerRow;
	Ogre::Real tilesPerCol;
	Ogre::Real tilesPerPlayer;
	
	Ogre::Real floorLength;
	Ogre::Real floorPosition;
};

#endif // #ifndef __Room_h_
