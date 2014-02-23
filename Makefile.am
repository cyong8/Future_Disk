ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = MCP.h Simulator.h PlayerCamera.h Player.h Disk.h Target.h Room.h  Wall.h ControlListener.h GameObject.h OgreMotionState.h BaseApplication.h 

bin_PROGRAMS = TronGame
TronGame_CPPFLAGS = -I$(top_srcdir)
TronGame_SOURCES = MCP.cpp Simulator.cpp PlayerCamera.cpp Player.cpp Disk.cpp Target.cpp  GameObject.cpp Room.cpp Wall.cpp ControlListener.cpp  BaseApplication.cpp 
TronGame_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS)
TronGame_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
