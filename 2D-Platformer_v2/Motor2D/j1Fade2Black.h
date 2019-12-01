#ifndef __FADETOBLACK_H__
#define __FADETOBLACK_H__

#include "j1Module.h"
#include "j1Audio.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "SDL\include\SDL_rect.h"

class j1Fade2Black : public j1Module
{
public:
	j1Fade2Black();
	~j1Fade2Black();

	bool Start();
	bool Update(float dt);
	bool FadeToBlack(j1Module* module_off, j1Module* module_on, float time = 2.0f);
	bool IsFading = false;

private:

	j1Module* moduleOff = nullptr;
	j1Module* moduleOn = nullptr;


	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen = { 0,0,0,0 };


};

#endif
#pragma once
