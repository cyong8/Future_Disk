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
	double dimension = 10.0f;
	Ogre::Vector3 *xzplane = new Ogre::Vector3(dimension*2, 0.01f, dimension*2);
	Ogre::Vector3 *yzplane = new Ogre::Vector3(0.01f, dimension, dimension*2);
	Ogre::Vector3 *xyplane = new Ogre::Vector3(dimension*2, dimension, 0.01f);
	
	(new Wall("Floor", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), *xzplane, Ogre::Vector3(0.0f, -dimension/2, 0.0f)))->addToSimulator();
	(new Wall("Ceiling", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.0f, 1.0f), *xzplane, Ogre::Vector3(0.0f, dimension/2, 0.0f)))->addToSimulator();
	(new Wall("wall1", mSceneMgr, game_simulator, Ogre::Vector3(0.0f, 1.0f, 1.0f), *yzplane, Ogre::Vector3(-dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall2", mSceneMgr, game_simulator, Ogre::Vector3(0.0f, 1.0f, 1.0f), *yzplane, Ogre::Vector3(dimension, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall3", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 1.0f, 0.0f), *xyplane, Ogre::Vector3(0.0f, 0.0f, -dimension)))->addToSimulator();
	(new Wall("wall4", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 1.0f, 0.0f), *xyplane, Ogre::Vector3(0.0f, 0.0f, dimension)))->addToSimulator();
	/*
	(new Wall("wall5", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall6", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall7", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall8", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	*/
}
