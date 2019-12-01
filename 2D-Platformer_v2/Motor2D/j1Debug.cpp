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
	if (input == true)
	{
		//Single map selection ------------------------------------------------------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
		{
			App->scene->destination_level = "maps/A1.tmx";
			CallFade();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN)
		{
			App->scene->destination_level = "maps/A2.tmx";
			CallFade();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN)
		{
			App->scene->destination_level = "maps/A3.tmx";
			CallFade();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_5) == KEY_DOWN)
		{
			App->scene->destination_level = "maps/A5.tmx";
			CallFade();
		}
		if (App->input->GetKey(SDL_SCANCODE_KP_6) == KEY_DOWN)
		{
			App->scene->destination_level = "maps/A6.tmx";
			CallFade();
		}

		//Swaps to lvl 1 --------------------------------------------------------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
			App->scene->destination_level = "maps/A1.tmx";
			CallFade();
		}
		
		//Swaps to lvl2 ---------------------------------------------------------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
			App->scene->destination_level = "maps/A3.tmx";
			CallFade();
		}

		//Resets current lvl ----------------------------------------------------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
			CallFade();
		}
		
		//Enables god mode ------------------------------------------------------------------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			//GOD MODE
		}
		if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
		{
			App->tiles->show_path = !App->tiles->show_path;
		}

		//Show colliders --------------------------------------------------------------------------------------------------
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
		if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		{
			App->maxcapFrames = !App->maxcapFrames;
		}
	}
	return true;
}

void j1Debug::CallFade()
{
	App->fade2black->FadeToBlack(App->scene, App->scene);
}
