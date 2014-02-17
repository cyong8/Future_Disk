#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim) 
	: GameObject(nym, mgr, sim)
{
	heldBy = NULL;
}

void Disk::setPlayer(Player* p)
{
	heldBy = p;
}