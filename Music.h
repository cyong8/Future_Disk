#ifndef __Music_h_
#define __Music_h_
//-lSDL_mixer

#include "SDL.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;

class Music
{
public:
	Music(void);
	virtual ~Music();
	void musicDone();
	void playCollisionSound(string objA, string objB);
	void playMusic(string state);
	
private:
	Mix_Chunk *startStopScreen;
	Mix_Chunk *playScreen;
	Mix_Chunk *diskToWall;
	Mix_Chunk *diskToTarget;
	Mix_Chunk *catchSound;
	Mix_Chunk *throwSound;

	int currentChunkChannel;
	int currentMusicChannel;
};

#endif // #ifndef __Music_h_