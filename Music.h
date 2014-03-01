#ifndef __Music_h_
#define __Music_h_

// SDL headers
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
	Mix_Chunk *pause_StartMusic;
	Mix_Chunk *gameMusic;
	Mix_Chunk *diskToWall;
	Mix_Chunk *diskToPlayer;
	Mix_Chunk *diskToTarget;

	int currentChunkChannel;
	int currentMusicChannel;
};

#endif // #ifndef __Music_h_