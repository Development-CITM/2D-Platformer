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
#include "j1Input.h"
#include "SDL/include/SDL.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include <math.h>

j1Fade2Black::j1Fade2Black():j1Module()
{
}

j1Fade2Black::~j1Fade2Black()
{}

bool j1Fade2Black::Start()
{
	screen = { 0,0,App->win->GetWidth() * App->win->GetScale(),App->win->GetHeight() * App->win->GetScale() };

	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	IsFading = false;
	return true;
}

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
		//App->input->Paused = true;
		if (now >= total_time)
		{
			//Fades music for 1 sec
			Mix_FadeOutMusic(1000);
			//Enables called module, disables current module


			moduleOff->Disable();
			moduleOn->Enable();


			//resets player & camera position
			App->render->camera.x = 0;
			App->render->camera.y = 0;

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
			current_step = fade_step::none;
		}
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool j1Fade2Black::FadeToBlack(j1Module* module_off, j1Module* module_on, float time)
{
	moduleOff = module_off;
	moduleOn = module_on;

	IsFading = true;

	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
	return true;
}
