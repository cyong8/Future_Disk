#ifndef __Tile_h_
#define __Tile_h_

#include "GameObject.h"

class Tile : public GameObject
{
public:
	Tile(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 position, Ogre::Real tileSize, int tileNum, int pID);
	int getTileNumber() { return tileNumber; };
	int getTileOwner() { return tileOwner; };

private:	
	Ogre::Vector3 dims;
	Ogre::Vector3 pos;

	Ogre::ParticleSystem* explosionParticle;
	Ogre::SceneNode* explosionNode;

	int tileNumber;
	int tileOwner;
};

#endif // #ifndef __Tile_h_
