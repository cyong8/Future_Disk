#ifndef __Music_h_
#define __Music_h_

class Music
{
	Mix_Chunk *collisionSound;
	Mix_Music *music;

	void musicDone();
};

#endif // #ifndef __Music_h_