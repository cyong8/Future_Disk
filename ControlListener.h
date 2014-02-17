#ifndef __ControlListener_h_
#define __ControlListener_h_

#include "BaseApplication.h"

class ControlListener : public BaseApplication
{
    public:
	ControlListener(void);
	virtual ~ControlListener(void);

    protected:
	virtual void keyPressed(const OIS::KeyEvent &arg);
	virtual void keyReleased(const OIS::KeyEvent &arg);
}
