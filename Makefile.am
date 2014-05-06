ACLOCAL_AMFLAGS= -I m4
noinst_HEADERS = Server.h Client.h Simulator.h Room.h Player.h Network.h Solo.h MCP.h Disk.h GUI.h Target.h GameObject.h OgreMotionState.h Wall.h Tile.h Music.h PlayerCamera.h BaseApplication.h 
bin_PROGRAMS = FutureDisk
FutureDisk_CPPFLAGS = -I$(top_srcdir) -I/usr/local/include/ -I/usr/local/lib
FutureDisk_SOURCES = Server.cpp Client.cpp Simulator.cpp Room.cpp Player.cpp Network.cpp Solo.cpp MCP.cpp Disk.cpp GUI.cpp  Target.cpp GameObject.cpp Wall.cpp Tile.cpp Music.cpp PlayerCamera.cpp BaseApplication.cpp  
FutureDisk_CXXFLAGS = $(OGRE_CFLAGS) $(OIS_CFLAGS) $(bullet_CFLAGS) $(sdl_CFLAGS) $(SDLMIXER_CFLAGS) $(SDLNET_CFLAGS) $(CEGUI_CFLAGS) $(CEGUI_OGRE_CFLAGS)
FutureDisk_LDADD = $(OGRE_LIBS) $(OIS_LIBS) $(bullet_LIBS) $(sdl_LIBS) $(SDLMIXER_LIBS) $(SDLNET_LIBS) $(CEGUI_LIBS) $(CEGUI_OGRE_LIBS)

EXTRA_DIST = buildit makeit
AUTOMAKE_OPTIONS = foreign
