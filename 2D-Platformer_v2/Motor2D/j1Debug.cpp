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


j1Debug::j1Debug() :j1Module()
{
	name.create("debug");
}

j1Debug::~j1Debug()
{}

bool j1Debug::Update(float dt)
{
	//Basic debug functions
	//Swap to lvl1
	if (input == true)
	{
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			App->fade2black->FadeToBlack(App->scene, App->scene);

		}
		if (App->input->GetKey(SDL_SCANCODE_KP_1))
		{
			App->scene->destination_level = "maps/A1.tmx";
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_2))
		{
			App->scene->destination_level = "maps/A2.tmx";
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_3))
		{
			App->scene->destination_level = "maps/A3.tmx";
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_5))
		{
			App->scene->destination_level = "maps/A5.tmx";
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_6))
		{
			App->scene->destination_level = "maps/A6.tmx";
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}

		//Swap to lvl2
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			App->fade2black->FadeToBlack(App->scene, App->scene);

		}

		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		{
			App->fade2black->FadeToBlack(App->scene, App->scene);
		}

		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{

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
	}
	return true;
}
