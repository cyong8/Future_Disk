#ifndef __GUI_h_
#define __GUI_h_

#include "MCP.h"

class MCP;

class GUI {
public:
    GUI(MCP* mcp);
    ~GUI(void);
    void createMainMenu(void);
    void pauseMenu(bool pause);
    bool createMultiplayerMenu(const CEGUI::EventArgs &e);
    bool enterIPAddress(const CEGUI::EventArgs &e);
    void gameOverScreen(void);
    void otherGameOverScreen(void);
    void addPanel(OgreBites::ParamsPanel* p, OgreBites::TrayLocation loc);
    void removePanel(OgreBites::ParamsPanel* p);
    void addLabel(OgreBites::Label* l, OgreBites::TrayLocation loc);
    void removeLabel(OgreBites::Label* l);
protected:
    CEGUI::OgreRenderer* mRenderer;
    MCP* mcp;
};

#endif // #ifndef __GUI_h_
