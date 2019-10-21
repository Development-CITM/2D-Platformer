#ifndef __ANIMATION_H__
#define __ANIMATION_H__
#include "p2SString.h"
#include "SDL/include/SDL_rect.h"

struct Animation {
	int numRects = 0;
	int numFrame = 0;
	int repeatFrames = 0;

	p2SString name;

	SDL_Rect* rects = nullptr;

	bool loop = false;

	void ResetAnim() {
		numFrame = 0;
	}
};

#endif // !__ANIMATION_H__
