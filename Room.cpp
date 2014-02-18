#include "Room.h"

Room::Room(Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	//wallList = vector<Wall>();  // might be unnecessary
	//Wall * wallh = (new Wall("Floor", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)));
	//addWall(wallh);
	addWalls(wallList,mSceneMgr,game_simulator);
}

void Room::addWall(Wall *w)
{
	((*this).wallList).push_back(*w);
}

void Room::addWalls(vector<Wall> wallList, Ogre::SceneManager *mSceneMgr, Simulator *game_simulator)
{
	double dimension = 1.0f;
	Ogre::Vector3 *xzplane = new Ogre::Vector3(dimension, dimension/100.0f, dimension);
	Ogre::Vector3 *yzplane = new Ogre::Vector3(dimension/100.0f, dimension, dimension);
	Ogre::Vector3 *xyplane = new Ogre::Vector3(dimension, dimension, dimension/100.0f);
	
	(new Wall("Floor", mSceneMgr, game_simulator, *xzplane, Ogre::Vector3(0.0f, -dimension, 0.0f)))->addToSimulator();
	(new Wall("Ceiling", mSceneMgr, game_simulator, *xzplane, Ogre::Vector3(0.0f, dimension, 0.0f)))->addToSimulator();
	(new Wall("wall1", mSceneMgr, game_simulator, *yzplane, Ogre::Vector3(-dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall2", mSceneMgr, game_simulator, *yzplane, Ogre::Vector3(dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall3", mSceneMgr, game_simulator, *xyplane, Ogre::Vector3(0.0f, 0.0f, -dimension)))->addToSimulator();
	(new Wall("wall4", mSceneMgr, game_simulator, *xyplane, Ogre::Vector3(0.0f, 0.0f, dimension)))->addToSimulator();
	/*
	(new Wall("wall5", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall6", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall7", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall8", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	*/
}
