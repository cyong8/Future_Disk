#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	//wallList = vector<Wall>();  // might be unnecessary
	//Wall * wallh = (new Wall("Floor", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)));
	//addWall(wallh);

	// Get the number of walls and roomSize
	// For the number of walls
		// Create wall of size 

	// HACK
	addWalls(wallList,mSceneMgr,game_simulator);
}
/*
	A method which adds a wall to the room
*/

void Room::addWall(Wall *w)
{
	((*this).wallList).push_back(*w);
}

/* 
	Adds a number of walls to a room - this is a hack
*/

void Room::addWalls(vector<Wall> wallList, Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	double dimension = 10.0f;
	Ogre::Vector3 *xzplane = new Ogre::Vector3(dimension*2, 0.01f, dimension*2);
	Ogre::Vector3 *yzplane = new Ogre::Vector3(0.01f, dimension, dimension*2);
	Ogre::Vector3 *xyplane = new Ogre::Vector3(dimension*2, dimension, 0.01f);
	
	// Specify name, axis it needs to be along, how big the wall needs to be, 
	//new Wall("WallExample", )

	(new Wall("Floor", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), *xzplane, Ogre::Vector3(0.0f, -dimension/2, 0.0f)))->addToSimulator();
	(new Wall("Ceiling", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), *xzplane, Ogre::Vector3(0.0f, dimension/2, 0.0f)))->addToSimulator();
	(new Wall("leftwall", mSceneMgr, game_simulator, Ogre::Vector3(0.0f, 1.0f, 2.0f), *yzplane, Ogre::Vector3(-dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("rightwall", mSceneMgr, game_simulator, Ogre::Vector3(0.0f, 1.0f, 2.0f), *yzplane, Ogre::Vector3(dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("backwall", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 1.0f, 0.0f), *xyplane, Ogre::Vector3(0.0f, 0.0f, -dimension * 2)))->addToSimulator();
	(new Wall("frontwall", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 1.0f, 0.0f), *xyplane, Ogre::Vector3(0.0f, 0.0f, dimension * 2)))->addToSimulator();

	/*
	(new Wall("wall5", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall6", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall7", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall8", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	*/
}

/*
	void returnBounds(void);
	Returns the outside bounds in the x direction
*/
