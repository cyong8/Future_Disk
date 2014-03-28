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
	virtual ~Music(void);
	void musicDone(void);
	void playCollisionSound(string objA, string objB);
	void playMusic(string state);
	void toggleMute(void);
	
private:
	// Triggered Music/Sounds
	Mix_Chunk *startStopScreen;
	Mix_Chunk *playScreen;
	Mix_Chunk *throwSound;
	Mix_Chunk *playerJump;
	Mix_Chunk *superJump;
	Mix_Chunk *speedUp;
	Mix_Chunk *blast;
	Mix_Chunk *bigBlast;

	// Collision Sounds
	Mix_Chunk *diskToWall;
	Mix_Chunk *diskToTarget;
	Mix_Chunk *catchSound;
	Mix_Chunk *playerLand;
	Mix_Chunk *restoreSound;
	
	int currentWallChannel;
	int currentJumpChannel;
	int currentMusicChannel;

	Mix_Chunk* currentMusic;

	bool muted;
};

#endif // #ifndef __Music_h_
