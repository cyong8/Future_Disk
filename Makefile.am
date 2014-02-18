ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = Room.h MCP.h Simulator.h Player.h Disk.h Wall.h ControlListener.h GameObject.h OgreMotionState.h BaseApplication.h 

bin_PROGRAMS = TronGame
TronGame_CPPFLAGS = -I$(top_srcdir)
TronGame_SOURCES = GameObject.cpp Simulator.cpp Room.cpp MCP.cpp Player.cpp Disk.cpp Wall.cpp ControlListener.cpp  BaseApplication.cpp 
TronGame_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS)
TronGame_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
