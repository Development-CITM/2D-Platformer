#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include "p2SString.h"
#include "SDL/include/SDL_rect.h"
#include "j1App.h"

struct Sprite {

	SDL_Rect rect;
	SDL_Rect AABB_rect;
	int frames = 0;
	int current_frame = 0;
	p2Point<int> AABB_offset;


};

struct Animation {
	bool loop = true;
	p2SString name;

	int num_sprites = 0;
	Sprite* sprites = nullptr;
	bool finished = false;

	p2Point<int> offset;

	int current_sprite = 0;
public:
	void ResetAnim() {
		current_sprite = 0;
		finished = false;
		for (int i = 0; i < num_sprites; i++)
		{
			sprites[i].current_frame = 0;
		}
	}

	int GetSprite(float dt)
	{
		if (sprites[current_sprite].current_frame != sprites[current_sprite].frames) 
		{
			if (App->maxcapFrames)
			{
				sprites[current_sprite].current_frame = sprites[current_sprite].current_frame + ceil(dt);//+ (int)(ceil(1 * dt * 50));
			}
			else
			{
				sprites[current_sprite].current_frame = sprites[current_sprite].current_frame + ceil(dt / 10000);//+ (int)(ceil(1 * dt * 50));
			}
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
			current_sprite = num_sprites - ceil(1*dt);
			finished = true;
			
		}

		return current_sprite;
	}
	
};

#endif // !__ANIMATION_H__
