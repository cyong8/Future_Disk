#include "Music.h"

Music::Music()
{
    // Initialize SDL
    SDL_Init(SDL_INIT_AUDIO);

  	int audio_rate = 22050;
  	Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
  	int audio_channels = 2;
  	int audio_buffers = 4096;
    muted = false;

    // Initialize SDL_mixer
    if(Mix_OpenAudio(audio_rate, MIX_DEFAULT_FORMAT, audio_channels, audio_buffers)) 
    {
    	printf("\n\n\n\n\n\nUnable to open audio!\n\n\n\n\n");
    	exit(1);
  	}

    // Initialise music files
        /**************** TRIGGERED MUSIC/SOUNDS ****************/
    startStopScreen = Mix_LoadWAV("music/altStartPause.wav"); // http://www.freesound.org/people/Dneproman/sounds/209561/
    if (startStopScreen == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING PAUSE MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        Mix_VolumeChunk(startStopScreen, 60);   // Volume ranges from 0 to 128
    }
    playScreen = Mix_LoadWAV("music/playScreen.wav");   // http://www.freesound.org/people/Erokia/sounds/163971/
    if (playScreen == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING PLAY MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        Mix_VolumeChunk(playScreen, 60);
    }
    throwSound = Mix_LoadWAV("music/throwDisk.wav"); // http://www.freesound.org/people/djgriffin/sounds/21998/
    if (throwSound == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING THROW MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        Mix_VolumeChunk(throwSound, 70);
    }
    playerJump = Mix_LoadWAV("music/jumpSound.wav"); // http://www.freesound.org/people/soundnimja/sounds/173326/
    if (playerJump == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING JUMP MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(playerJump, 70);
    }
    superJump = Mix_LoadWAV("music/superJump.wav"); //http://www.freesound.org/people/coby12388/sounds/222571/
    if (superJump == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING SUPER JUMP MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(superJump, 70);
    }
    speedUp = Mix_LoadWAV("music/speed.wav"); //http://www.freesound.org/people/LloydEvans09/sounds/185849/
    if (speedUp == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING SPEED MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(speedUp, 70);
    }
    blast = Mix_LoadWAV("music/blast.wav"); //http://www.freesound.org/people/Benboncan/sounds/73005/
    if (blast == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING BLAST MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(blast, 70);
    }
    bigBlast = Mix_LoadWAV("music/bigBlast.wav"); //http://www.freesound.org/people/klangfabrik/sounds/210613/
    if (bigBlast == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING BIG BLAST MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(bigBlast, 70);
    }

        /**************** COLLISION SOUNDS ****************/
    diskToWall = Mix_LoadWAV("music/BalltoWall.wav");	// http://www.freesound.org/people/davidou/sounds/88451/
    if (diskToWall == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING BALL-WALL MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        Mix_VolumeChunk(diskToWall, 70);
    }
    
	diskToTarget = Mix_LoadWAV("music/TargetHit.wav"); // http://www.freesound.org/people/fins/sounds/171671/
    if (diskToTarget == NULL)
    {
    	printf("\n\n\n\n\nNOT LOADING TARGET MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(diskToTarget, 70);
    }
    catchSound = Mix_LoadWAV("music/catchDisk.wav");     // http://www.freesound.org/people/grunz/sounds/109663/
    if (catchSound == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING CATCH MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(catchSound, 70);
    } 
    playerLand = Mix_LoadWAV("music/playerLand.wav"); //http://www.freesound.org/people/odeean/sounds/139476/
    if (playerLand == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING LAND MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(playerLand, 70);
    }
    restoreSound = Mix_LoadWAV("music/restore.wav"); //http://www.freesound.org/people/Fantom57/sounds/219230/
    if (restoreSound == NULL)
    {
        printf("\n\n\n\n\nNOT LOADING RESTORE MUSIC FILE\n\n\n\n\n");
    }
    else
    {
        // Set Volume
        Mix_VolumeChunk(restoreSound, 70);
    }
    // Initialise sound channels
    currentWallChannel = 0;
    currentJumpChannel = 2;
	currentMusicChannel = 1;
}
//-------------------------------------------------------------------------------------
Music::~Music()
{
}
//-------------------------------------------------------------------------------------
void Music::musicDone()
{
    Mix_FreeChunk(startStopScreen);
    Mix_FreeChunk(playScreen);
    Mix_FreeChunk(throwSound);
    Mix_FreeChunk(playerJump);
    Mix_FreeChunk(superJump);
    Mix_FreeChunk(speedUp);
    Mix_FreeChunk(blast);
    Mix_FreeChunk(bigBlast);

	Mix_FreeChunk(diskToWall);
    Mix_FreeChunk(diskToTarget);
    Mix_FreeChunk(catchSound);
    Mix_FreeChunk(playerLand);
    Mix_FreeChunk(restoreSound);

    Mix_CloseAudio();
    SDL_Quit();
}
//-------------------------------------------------------------------------------------
void Music::playCollisionSound(string objA, string objB)
{
    if (!muted)
    {
    	// Disk To Wall
    	if (objA == "Disk" && objB == "Player")
    	{
    		Mix_PlayChannel(-1, catchSound, 0);
    	}
    	// Disk To Player
    	if (objA == "Disk" && objB == "Wall")
    	{
    		currentWallChannel = Mix_PlayChannel(currentWallChannel, diskToWall, 0);
    	}
    	// Disk To Target
    	if (objA == "Disk" && objB == "Target")
    	{
    		Mix_PlayChannel(-1, diskToTarget, 0);
    	}
        // Player to Ground
        if (objA == "Player" && objB == "Ground")
        {
            Mix_PlayChannel(-1, playerLand, 0);
        }
    	SDL_Delay(50);
    }
}
//-------------------------------------------------------------------------------------
void Music::playMusic(string state)
{
    if (!muted)
    {
        // Start/Pause Menu Music
    	if (state == "Pause" || state == "Start")
    	{
            currentMusic = startStopScreen;
            Mix_FadeOutChannel(currentMusicChannel, 3000);
            currentMusicChannel = Mix_FadeInChannel(currentMusicChannel, startStopScreen, -1, 1500);//Mix_PlayChannel(-1, startStopScreen, -1);
        }
        // Gameplay Music
    	if (state == "Play")
    	{
            currentMusic = playScreen;
    		Mix_FadeOutChannel(currentMusicChannel, 3000);
            currentMusicChannel = Mix_FadeInChannel(currentMusicChannel, playScreen, -1, 1500);//Mix_PlayChannel(-1, playScreen, -1);
    	}
        // Throwing Sound
        if (state == "Throw")
        {
            Mix_PlayChannel(-1, throwSound, 0);
        }
        // Player Jumping
        if (state == "Jump")
        {
            currentJumpChannel = Mix_PlayChannel(currentJumpChannel, playerJump, 0);
        }
        // Player Super Jumping
        if (state == "SuperJump")
        {
            currentJumpChannel = Mix_PlayChannel(currentJumpChannel, superJump, 0);
        }
        // Restore sound
        if (state == "Restore")
        {
            Mix_PlayChannel(-1, restoreSound, 0);
        }
        // Disk Speed Up
        if (state == "SpeedUp")
        {
            Mix_PlayChannel(-1, speedUp, 0);
        }
        // Tile Destroyed
        if (state == "Blast")
        {
            Mix_PlayChannel(-1, blast, 0);
        }
        // Multiple Tiles Destroyed
        if (state == "BigBlast")
        {
            Mix_PlayChannel(-1, bigBlast, 0);
        }
    	SDL_Delay(50);
    }
}
//-------------------------------------------------------------------------------------
void Music::toggleMute(void)
{
    if (muted) // Unmute the Music 
        Mix_PlayChannel(currentMusicChannel, currentMusic, -1);
    else // Mute the Music
        Mix_HaltChannel(-1);

    muted = !muted;
}
