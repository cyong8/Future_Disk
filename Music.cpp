#include "Music.h"


Music::Music()
{
    SDL_Init(SDL_INIT_AUDIO);

  	int audio_rate = 22050;
  	Uint16 audio_format = AUDIO_S16; 
  	int audio_channels = 2;
  	int audio_buffers = 4096;

    Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);

    // Initialise music files
    pause_StartMusic = Mix_LoadMUS("Pause_StartScreen.wav"); // http://www.freesound.org/people/Erokia/sounds/179287/
    gameMusic = Mix_LoadMUS("PlayScreen.wav");   // http://www.freesound.org/people/Erokia/sounds/218854/
    diskToWall = Mix_LoadWAV("BalltoWall.wav");	 // http://www.freesound.org/people/qubodup/sounds/157609/
    diskToPlayer = Mix_LoadWAV("Catch.wav");     // http://www.freesound.org/people/jorickhoofd/sounds/170623/
    diskToTarget = Mix_LoadWAV("TargetHit.wav"); // http://www.freesound.org/people/fins/sounds/171671/
    // MORE MUSIC FOR PLAYER MOVEMENT?

    // Initialise sound channels
    currentChunkChannel = -1;
	currentMusicChannel = -1;
}

Music::~Music()
{
}

void Music::musicDone()
{
	Mix_FreeChunk(diskToWall);
	Mix_FreeChunk(diskToPlayer);
    Mix_FreeChunk(diskToTarget);
    Mix_FreeMusic(pause_StartMusic);
    Mix_FreeMusic(gameMusic);

    SDL_Quit();
}

void Music::playCollisionSound(string objA, string objB)
{
	// Disk To Wall
	if ((objA == "Disk" && objB == "Player") || (objA == "Player" && objA == "Disk"))
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(-1, phaser, 0);
	}
	// Disk To Player
	if (objA == "Disk" && objB == "Wall")
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(-1, phaser, 0);
	}
	// Disk To Target
	if (objA == "Disk" && objB == "Target")
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(-1, phaser, 0);
	}
}
void Music::playMusic(string state)
{
	if (state == "Pause" || state == "Start")
	{
		Mix_HaltChannel(currentMusicChannel);
		currentMusicChannel = Mix_PlayChannel(-1, phaser, -1);
	}
	if (state == "Play")
	{
		Mix_HaltChannel(currentMusicChannel);
		currentMusicChannel = Mix_PlayChannel(-1, phaser, -1);
	}
}