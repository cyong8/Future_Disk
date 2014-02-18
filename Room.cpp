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
	(new Wall("Floor", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("Ceiling", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 10.0f, 0.0f)))->addToSimulator();
	/*
	(new Wall("wall1", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall2", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall3", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall4", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall5", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall6", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall7", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	(new Wall("wall8", mSceneMgr, game_simulator, Ogre::Vector3(1.0f, 0.01f, 1.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)))->addToSimulator();
	*/
}
