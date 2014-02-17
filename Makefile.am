ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = MCP.h BaseApplication.h OgreMotionState.h Simulator.h GameObject.h ControlListener.h Player.h Disk.h Wall.h Room.h

bin_PROGRAMS = TronGame
TronGame_CPPFLAGS = -I$(top_srcdir)
TronGame_SOURCES = MCP.cpp BaseApplication.cpp Simulator.cpp GameObject.cpp ControlListener.cpp Player.cpp Disk.cpp Wall.cpp Room.cpp
TronGame_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS)
TronGame_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
