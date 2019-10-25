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
#include "j1Debug.h"


j1Debug::j1Debug():j1Module()
{}

j1Debug::~j1Debug()
{}

bool j1Debug::Update(float dt)
{
	//Basic debug functions
	//Swap to lvl1
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->fade2black->FadeToBlack(App->scene, App->scene); 
		App->scene->lvl2 = false;
		App->scene->lvl1 = true;
	}

	//Swap to lvl2
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->fade2black->FadeToBlack(App->scene, App->scene); 
		App->scene->lvl1 = false;
		App->scene->lvl2 = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->fade2black->FadeToBlack(App->scene, App->scene);
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if(App->player->god_mode_enabled == false)
			App->player->god_mode_enabled = true;
		else if (App->player->god_mode_enabled == true)
			App->player->god_mode_enabled = false;
	}

	//Show colliders
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		if (App->collider->collider_debug == false)
		{
			App->collider->collider_debug = true;
		}
		else if (App->collider->collider_debug == true)
		{
			App->collider->collider_debug = false;
		}
	}
	return true;
}
