#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Fade2Black.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Debug.h"
#include "j1Input.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include <math.h>

#pragma region Constructor/Destructor
j1Fade2Black::j1Fade2Black():j1Module()
{
	name.create("fade2Black");
}

j1Fade2Black::~j1Fade2Black()
{}
#pragma endregion

#pragma region Start
bool j1Fade2Black::Start()
{
	screen = { 0,0,App->win->GetWidth() * App->win->GetScale(),App->win->GetHeight() * App->win->GetScale() };

	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	IsFading = false;
	return true;
}
#pragma endregion

#pragma region Update
bool j1Fade2Black::Update(float dt)
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			//Fades music for 1 sec
			Mix_FadeOutMusic(1000);

			//Enables called module, disables current module
			moduleOff->Disable();
			moduleOn->Enable();
			App->scene->loading = false;

			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;
		if (now >= total_time)
		{
			IsFading = false;
			App->debug->input = true;
			current_step = fade_step::none;
		}
	} break;
	}
	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}
#pragma endregion

#pragma region Fade Function
bool j1Fade2Black::FadeToBlack(j1Module* module_off, j1Module* module_on, float time)
{
	bool ret = false;
	moduleOff = module_off;
	moduleOn = module_on;

	IsFading = true;

	App->debug->input = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 500);
		ret = true;
	}

	return ret;
}
#pragma endregion