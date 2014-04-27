#include "GUI.h"

GUI::GUI(MCP* m) 
{
    mcp = m;
    mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    
    CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
    
    CEGUI::SchemeManager::getSingleton().create("OgreTray.scheme");
    CEGUI::System::getSingleton().setDefaultMouseCursor("OgreTrayImages", "MouseArrow");
    CEGUI::MouseCursor::getSingleton().setPosition(CEGUI::Point(mcp->getRenderWindow()->getWidth()/2, mcp->getRenderWindow()->getHeight()/2));
}
//------------------------------------------------------------------------------------
GUI::~GUI() 
{

}
//------------------------------------------------------------------------------------
void GUI::createMainMenu() 
{           
    CEGUI::MouseCursor::getSingleton().show();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/MainMenu/Sheet");
 
    CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/MainMenu/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    
    /* SOLO MODE */
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("SoloModeImageset", "solobutton.png");
    CEGUI::Window *singlePlayerStartIMG = wmgr.createWindow("OgreTray/StaticImage", "TronGame/MainMenu/SinglePlayerStartButtonIMG");
    singlePlayerStartIMG->setProperty("Image", "set:SoloModeImageset image:full_image");
    singlePlayerStartIMG->setProperty("BackgroundEnabled", "false");
    singlePlayerStartIMG->setProperty("FrameEnabled", "false");
    singlePlayerStartIMG->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    singlePlayerStartIMG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.47, 0)));

    CEGUI::Window *singlePlayerStart = wmgr.createWindow("OgreTray/Button", "TronGame/MainMenu/SinglePlayerStartButton");
    // singlePlayerStart->setText("Solo Mode");
    singlePlayerStart->setAlpha(0.1f);
    singlePlayerStart->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    singlePlayerStart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.47, 0)));
    
    /* HOST GAME */
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("HostGameImageset", "hostgamebutton.png");
    CEGUI::Window *hostIMG = wmgr.createWindow("OgreTray/StaticImage", "TronGame/MainMenu/HostButtonIMG");
    hostIMG->setProperty("Image", "set:HostGameImageset image:full_image");
    hostIMG->setProperty("BackgroundEnabled", "false");
    hostIMG->setProperty("FrameEnabled", "false");
    hostIMG->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    hostIMG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.59, 0)));
    
    CEGUI::Window *host = wmgr.createWindow("OgreTray/Button", "TronGame/MainMenu/HostButton");
    // host->setText("Host Game");
    host->setAlpha(0.1f);
    host->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    host->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.59, 0)));

    /* JOIN GAME */
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("JoinGameImageset", "joingamebutton.png");
    CEGUI::Window *joinIMG = wmgr.createWindow("OgreTray/StaticImage", "TronGame/MainMenu/JoinButtonIMG");
    joinIMG->setProperty("Image", "set:JoinGameImageset image:full_image");
    joinIMG->setProperty("BackgroundEnabled", "false");
    joinIMG->setProperty("FrameEnabled", "false");
    joinIMG->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    joinIMG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.71, 0)));
    
    CEGUI::Window *join = wmgr.createWindow("OgreTray/Button", "TronGame/MainMenu/JoinButton");
    // join->setText("Join a game");
    join->setAlpha(0.1f);
    join->setSize(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.08, 0)));
    join->setPosition(CEGUI::UVector2(CEGUI::UDim(0.1, 0), CEGUI::UDim(0.71, 0)));
    
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("TitleImageset", "title.png");
    CEGUI::Window *title = wmgr.createWindow("OgreTray/StaticImage", "TronGame/MainMenu/TitleImage");
    title->setProperty("Image", "set:TitleImageset image:full_image");
    title->setProperty("BackgroundEnabled", "false");
    title->setProperty("FrameEnabled", "false");
    title->setSize(CEGUI::UVector2(CEGUI::UDim(0.5, 0), CEGUI::UDim(0.3, 0)));
    title->setPosition(CEGUI::UVector2(CEGUI::UDim(0.25, 0), CEGUI::UDim(0.05, 0)));
    
    CEGUI::ImagesetManager::getSingleton().createFromImageFile("InstructionsImageset", "controlmenu.png");
    CEGUI::Window *instructions = wmgr.createWindow("OgreTray/StaticImage", "TronGame/MainMenu/InstructionsImage");
    instructions->setProperty("Image", "set:InstructionsImageset image:full_image");
    instructions->setProperty("BackgroundEnabled", "false");
    instructions->setProperty("FrameEnabled", "false");
    instructions->setSize(CEGUI::UVector2(CEGUI::UDim(0.6, 0), CEGUI::UDim(0.6, 0)));
    instructions->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.38, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(singlePlayerStartIMG);
    sheet->addChildWindow(singlePlayerStart);
    sheet->addChildWindow(hostIMG);
    sheet->addChildWindow(host);
    sheet->addChildWindow(joinIMG);
    sheet->addChildWindow(join);
    sheet->addChildWindow(title);
    sheet->addChildWindow(instructions);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
    singlePlayerStart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::soloMode, mcp));
    host->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::hostGame, mcp));
    join->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GUI::enterIPAddress, this));
}
//---------------------------------------------------------------------------------------
void GUI::pauseMenu(bool pause)
{
    if (pause) {
        CEGUI::MouseCursor::getSingleton().hide();
        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
        wmgr.destroyAllWindows();
    }
    else {
        CEGUI::MouseCursor::getSingleton().show();
        CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
        CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/Pause/Sheet");
        
        CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/Pause/QuitButton");
        quit->setText("Quit Game");
        quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        
        CEGUI::Window *restart = wmgr.createWindow("OgreTray/Button", "TronGame/Pause/RestartGameButton");
        restart->setText("Restart Game");
        restart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        restart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.6, 0)));
        
        CEGUI::Window *back = wmgr.createWindow("OgreTray/Button", "TronGame/Pause/BackButton");
        back->setText("Back to Main Menu");
        back->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
        back->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38, 0), CEGUI::UDim(0.7, 0)));
        
        sheet->addChildWindow(quit);
        sheet->addChildWindow(restart);
        sheet->addChildWindow(back);
        
        CEGUI::System::getSingleton().setGUISheet(sheet);
    
        quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
        restart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::soloMode, mcp));
        back->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::activateMainMenuSolo, mcp));
    }
}
//-------------------------------------------------------------------------------------
bool GUI::createMultiplayerMenu(const CEGUI::EventArgs &e)
{
    addPanel(mcp->getPanel(POWER), OgreBites::TL_BOTTOMLEFT);

    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/MultiplayerMenu/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/MultiplayerMenu/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    
    CEGUI::Window *host = wmgr.createWindow("OgreTray/Button", "TronGame/MultiplayerMenu/HostButton");
    host->setText("Host a game");
    host->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    host->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.45, 0)));
    
    CEGUI::Window *join = wmgr.createWindow("OgreTray/Button", "TronGame/MultiplayerMenu/JoinButton");
    join->setText("Join a game");
    join->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    join->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.56, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(host);
    sheet->addChildWindow(join);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
}
//-------------------------------------------------------------------------------------
bool GUI::enterIPAddress(const CEGUI::EventArgs &e)
{
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/ClientMenu/Sheet");
    sheet->setUserString("NOTHING", "TronGame/ClientMenu/IPEditbox");
    
    CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/ClientMenu/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    quit->setUserString("NOTHING", "TronGame/ClientMenu/IPEditbox");
    
    CEGUI::Window *ip = wmgr.createWindow("OgreTray/Editbox", "TronGame/ClientMenu/IPEditbox");
    ip->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    ip->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.45, 0)));
    ip->setUserString("TronGame/ClientMenu/IPEditbox", "TronGame/ClientMenu/IPEditbox");
    
    CEGUI::Window *enter = wmgr.createWindow("OgreTray/Button", "TronGame/ClientMenu/EnterButton");
    enter->setText("Enter");
    enter->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    enter->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.55, 0)));
    enter->setUserString("NOTHING", "TronGame/ClientMenu/IPEditbox");
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(ip);
    sheet->addChildWindow(enter);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
    enter->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::joinGame, mcp));
}
//-------------------------------------------------------------------------------------
void GUI::gameOverScreen() 
{
    CEGUI::MouseCursor::getSingleton().show();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/GameOver1/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver1/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        
    CEGUI::Window *restart = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver1/RestartGameButton");
    restart->setText("Restart Game");
    restart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    restart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.6, 0)));
    
    CEGUI::Window *back = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver1/BackButton");
    back->setText("Back to Main Menu");
    back->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
    back->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38, 0), CEGUI::UDim(0.7, 0)));
    
    sheet->addChildWindow(quit);
    sheet->addChildWindow(restart);
    sheet->addChildWindow(back);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
    restart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::soloMode, mcp));
    back->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::activateMainMenuSolo, mcp));
}
//-------------------------------------------------------------------------------------
void GUI::otherGameOverScreen() 
{
    CEGUI::MouseCursor::getSingleton().show();
    CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
    wmgr.destroyAllWindows();
    CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "TronGame/GameOver2/Sheet");
    
    CEGUI::Window *quit = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver2/QuitButton");
    quit->setText("Quit Game");
    quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
        
    /*CEGUI::Window *restart = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver2/RestartGameButton");
    restart->setText("Restart Game");
    restart->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
    restart->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4, 0), CEGUI::UDim(0.6, 0)));
    
    CEGUI::Window *back = wmgr.createWindow("OgreTray/Button", "TronGame/GameOver2/BackButton");
    back->setText("Back to Main Menu");
    back->setSize(CEGUI::UVector2(CEGUI::UDim(0.20, 0), CEGUI::UDim(0.05, 0)));
    back->setPosition(CEGUI::UVector2(CEGUI::UDim(0.38, 0), CEGUI::UDim(0.7, 0)));*/
    
    sheet->addChildWindow(quit);
    //sheet->addChildWindow(restart);
    //sheet->addChildWindow(back);
    
    CEGUI::System::getSingleton().setGUISheet(sheet);
    
    quit->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::quit, mcp));
    //restart->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::soloMode, this));
    //back->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MCP::activateMainMenuSolo, this));
}
//--------------------------------------------------------------------------------------
void GUI::addPanel(OgreBites::ParamsPanel* p, OgreBites::TrayLocation loc)
{
    p->show();
    mcp->getTrayManager()->moveWidgetToTray(p, loc);
}
//--------------------------------------------------------------------------------------
void GUI::removePanel(OgreBites::ParamsPanel* p)
{
    p->hide();
    mcp->getTrayManager()->removeWidgetFromTray(p);
}
//--------------------------------------------------------------------------------------
void GUI::addLabel(OgreBites::Label* l, OgreBites::TrayLocation loc)
{
    l->show();
    mcp->getTrayManager()->moveWidgetToTray(l, loc);
}
//--------------------------------------------------------------------------------------
void GUI::removeLabel(OgreBites::Label* l)
{
    l->hide();
    mcp->getTrayManager()->removeWidgetFromTray(l);
}
