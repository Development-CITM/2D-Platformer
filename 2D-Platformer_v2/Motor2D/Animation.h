#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include "p2SString.h"
#include "SDL/include/SDL_rect.h"

struct Sprite {

	SDL_Rect rect;
	int frames = 0;

};

struct Animation {
	bool loop = false;
	p2SString name;

	Sprite* sprites = nullptr;

	p2Point<int> offset;
};

#endif // !__ANIMATION_H__
