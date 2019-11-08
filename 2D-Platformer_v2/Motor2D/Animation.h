#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include "p2SString.h"
#include "SDL/include/SDL_rect.h"

struct Sprite {

	SDL_Rect rect;
	int frames = 0;
	int current_frame = 0;

};

struct Animation {
	bool loop = true;
	p2SString name;

	int num_sprites = 0;
	Sprite* sprites = nullptr;

	p2Point<int> offset;

	int current_sprite = 0;

	int GetSprite()
	{
		if (sprites[current_sprite].current_frame != sprites[current_sprite].frames) 
		{
			sprites[current_sprite].current_frame++;
		}
		else {
			sprites[current_sprite].current_frame = 0;
			current_sprite++;
		}
		if (loop) {
			if (current_sprite == num_sprites) {
				current_sprite = 0;
			}
		}
		else if(current_sprite == num_sprites){
			current_sprite = num_sprites-1;
		}

		return current_sprite;
	}
	
};

#endif // !__ANIMATION_H__
