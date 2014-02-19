#include "GameObject.h"

class Target
{
public:
	Target(Ogre::String nym, Ogre::SceneManager *mgr, Simulator *sim, Ogre::Vector3 dimensions, Ogre::Vector3 iposition);
	void setTargetHit();

protected:
	bool hit;
}