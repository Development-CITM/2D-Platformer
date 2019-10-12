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
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_REPEAT)
	{
		App->fade2black->FadeToBlack(App->scene, App->scene); //TODO EUDALD: make sure what's not cleaning and clean it!
	}
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
