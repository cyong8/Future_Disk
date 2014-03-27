#ifndef __Tile_h_
#define __Tile_h_

#include "GameObject.h"

class Tile : public GameObject
{
public:
	Tile(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 position, Ogre::Real tileSize);
	Ogre::Vector3 dims;
	Ogre::Vector3 pos;
	void markHit();
	bool isHit();
	int indexIntoTileArray;
};

#endif // #ifndef __Tile_h_
