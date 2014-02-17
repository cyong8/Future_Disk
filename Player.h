#ifndef __Player_h_
#define __Player_h_

#include "GameObject.h"
#include "PlayerCamera.h"

class Player : public GameObject
{
public:
	Player();
	void setHolding();

protected:
	bool isHolding;
};

#endif // #ifndef __Player_h_