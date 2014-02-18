#ifndef __ControlListener_h_
#define __ControlListener_h_

#include "BaseApplication.h"

class ControlListener
{
    public:
	ControlListener(void);
	virtual ~ControlListener(void);

    protected:
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
};

#endif // #ifndef __ControlListener_h_