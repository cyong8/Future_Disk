ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = Music.h MCP.h Simulator.h PlayerCamera.h Player.h Disk.h Target.h Room.h  Wall.h GameObject.h OgreMotionState.h BaseApplication.h 
bin_PROGRAMS = TronGame
TronGame_CPPFLAGS = -I$(top_srcdir)
TronGame_SOURCES = Music.cpp MCP.cpp Simulator.cpp PlayerCamera.cpp Player.cpp Disk.cpp Target.cpp  GameObject.cpp Room.cpp Wall.cpp BaseApplication.cpp 
TronGame_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(sdl_CFLAGS) $(SDLMIXER_CFLAGS)
TronGame_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(sdl_LIBS) $(SDLMIXER_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
