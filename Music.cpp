#include "Music.h"


Music::Music()
{
    //SDL_Init(SDL_INIT_AUDIO);
    

    // if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )

    // music = Mix_LoadMUS( "test.wav");
    // collisionSound = Mix_LoadWAV("collide.wav");
}

Music::~Music()
{

}

void Music::musicDone()
{
	//Mix_FreeChunk(collisionSound);
    //Mix_FreeMusic(music);
    //SDL_Quit();
}