#ifndef __Disk_h_
#define __Disk_h_

#include "GameObject.h"

class Disk : public GameObject
{
public:
	Disk();
	void setPlayer(Player p); //player caught the disk

protected:
	Player heldBy;
};

#endif // #ifndef __Disk_h_