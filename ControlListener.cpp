#include "ControlListener.h"

ControlListener::ControlListener(void)
{	
}
ControlListener::~ControlListener(void)
{
}
//-------------------------------------------------------------------------------------
bool ControlListener::keyPressed( const OIS::KeyEvent &arg )
{
    switch(arg.key)
    {
    	
	case OIS::KC_V:
	//    changeView(Player p);
	    break;
	case OIS::KC_P:
	//    pause();
	    break;
	case OIS::KC_W:
		// Move character forward
		break;
	case OIS::KC_A:
		// Move character to the left
		break;
	case OIS::KC_S:
		// Move character backward
		break;
	case OIS::KC_D:
		// Move character to the right
		break;
	default:
	    break;
	
    }
    return true;
}

bool ControlListener::keyReleased(const OIS::KeyEvent &arg)
{

}
