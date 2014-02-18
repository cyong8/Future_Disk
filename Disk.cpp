#include "Disk.h"
#include "Player.h"

Disk::Disk(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Real height, Ogre::Real radius, Ogre::Vector3 iposition) 
	: GameObject(nym, mgr, sim)
{
	typeName = "Disk";
	heldBy = NULL;
}

void Disk::setPlayer(Player* p)
{
	heldBy = p;
}