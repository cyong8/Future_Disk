#include "ControlListener.h"

ControlListener::ControlListener(void){
	
}
ControlListener::~ControlListener(void)
{
}
//-------------------------------------------------------------------------------------
bool ControlListener::keyPressed( const OIS::KeyEvent &arg )
{
    switch(evt.key)
    {
	case OIS::KC_V:
	    changeView(Player p);
	    break;
	case OIS::KC_P:
	    pause();
	    break;
	default:
	    break;
	
    }
    return true;
}
