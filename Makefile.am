ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = Room.h Network.h Server.h Client.h Solo.h MCP.h Simulator.h GUI.h Player.h Disk.h Target.h GameObject.h OgreMotionState.h Wall.h Tile.h Music.h PlayerCamera.h BaseApplication.h 
bin_PROGRAMS = TronGame
TronGame_CPPFLAGS = -I$(top_srcdir) -I/usr/local/include/ -I/usr/local/lib
TronGame_SOURCES = Room.cpp Network.cpp Server.cpp Client.cpp Solo.cpp MCP.cpp Simulator.cpp GUI.cpp Player.cpp Disk.cpp Target.cpp GameObject.cpp Wall.cpp Tile.cpp Music.cpp PlayerCamera.cpp BaseApplication.cpp  
TronGame_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(sdl_CFLAGS) $(SDLMIXER_CFLAGS) $(SDLNET_CFLAGS) $(CEGUI_CFLAGS) $(CEGUI_OGRE_CFLAGS)
TronGame_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(sdl_LIBS) $(SDLMIXER_LIBS) $(SDLNET_LIBS) $(CEGUI_LIBS) $(CEGUI_OGRE_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
