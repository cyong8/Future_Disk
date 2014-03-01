#include "Music.h"

Music::Music()
{
    SDL_Init(SDL_INIT_AUDIO);

    // Mix_Chunk *startStopScreen = NULL;
    // Mix_Chunk *playScreen = NULL;
    // Mix_Chunk *diskToWall = NULL;
    // Mix_Chunk *catchsound = NULL;
    // Mix_Chunk *diskToTarget = NULL;

  	int audio_rate = 22050;
  	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  	int audio_channels = 2;
  	int audio_buffers = 4096;

    if(Mix_OpenAudio(audio_rate, MIX_DEFAULT_FORMAT, audio_channels, audio_buffers)) 
    {
    	printf("\n\n\n\n\n\nUnable to open audio!\n\n\n\n\n");
    	exit(1);
  	}

    // Initialise music files
    startStopScreen = Mix_LoadWAV("altStartPause.wav"); // http://www.freesound.org/people/Dneproman/sounds/209561/
    if (startStopScreen == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING PAUSE MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(startStopScreen, 40);   // Volume ranges from 0 to 128
    }
    playScreen = Mix_LoadWAV("playScreen.wav");   // http://www.freesound.org/people/Erokia/sounds/163971/
    if (playScreen == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING PLAY MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        Mix_VolumeChunk(playScreen, 40);
    }
    throwSound = Mix_LoadWAV("throwDisk.wav"); // http://www.freesound.org/people/djgriffin/sounds/21998/
    if (throwSound == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING THROW MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(throwSound, 50);
    }
    diskToWall = Mix_LoadWAV("BalltoWall.wav");	 // http://www.freesound.org/people/qubodup/sounds/157609/
    if (diskToWall == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING BALL-WALL MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(diskToWall, 50);
    }
    catchSound = Mix_LoadWAV("catchDisk.wav");     // http://www.freesound.org/people/grunz/sounds/109663/
    if (catchSound == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING CATCH MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(catchSound, 50);
    }
	diskToTarget = Mix_LoadWAV("TargetHit.wav"); // http://www.freesound.org/people/fins/sounds/171671/
    if (diskToTarget == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING TARGET MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(diskToTarget, 50);
    }

    // Initialise sound channels
    currentChunkChannel = 0;
	currentMusicChannel = 1;
}

Music::~Music()
{
}

void Music::musicDone()
{
    Mix_FreeChunk(startStopScreen);
    Mix_FreeChunk(playScreen);
	Mix_FreeChunk(diskToWall);
    Mix_FreeChunk(diskToTarget);
    Mix_FreeChunk(catchSound);
    Mix_FreeChunk(throwSound);
    Mix_CloseAudio();
    SDL_Quit();
}

void Music::playCollisionSound(string objA, string objB)
{
	// Disk To Wall
	if ((objA == "Disk" && objB == "Player") || (objA == "Player" && objA == "Disk"))
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(currentChunkChannel, catchSound, 0);
	}
	// Disk To Player
	if (objA == "Disk" && objB == "Wall")
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(currentChunkChannel, diskToWall, 0);
	}
	// Disk To Target
	if (objA == "Disk" && objB == "Target")
	{
		Mix_HaltChannel(currentChunkChannel);
		currentChunkChannel = Mix_PlayChannel(currentChunkChannel, diskToTarget, 0);
	}
	SDL_Delay(50);
}
void Music::playMusic(string state)
{
	if (state == "Pause" || state == "Start")
	{
		//Mix_HaltChannel(currentMusicChannel);
        Mix_FadeOutChannel(currentMusicChannel, 3000);
        currentMusicChannel = Mix_FadeInChannel(currentMusicChannel, startStopScreen, -1, 1500);//Mix_PlayChannel(-1, startStopScreen, -1);
    }
	if (state == "Play")
	{
		//Mix_HaltChannel(currentMusicChannel);
		Mix_FadeOutChannel(currentMusicChannel, 3000);
        currentMusicChannel = Mix_FadeInChannel(currentMusicChannel, playScreen, -1, 1500);//Mix_PlayChannel(-1, playScreen, -1);
	}
    if (state == "Throw")
    {
        Mix_HaltChannel(currentChunkChannel);
        currentChunkChannel = Mix_PlayChannel(currentChunkChannel, throwSound, 0);
    }
	SDL_Delay(50);
}