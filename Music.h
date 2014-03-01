#ifndef __Music_h_
#define __Music_h_

// SDL headers
#include "SDL.h"
#include "SDL_mixer.h"
#include <stdlib.h>

class Music
{
	Mix_Music *pauseMusic;
	Mix_Music *gameMusic;
	Mix_Chunk *diskToWall;
	Mix_Chunk *diskToPlayer;
	Mix_Chunk *diskToTarget;

	int currentChunkChannel;
	int currentMusicChannel;

	void musicDone();
	void playCollisionSound(string objA, string objB);
	void playMusic(string state);
};

#endif // #ifndef __Music_h_