#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Tilesets.h"
#include "j1Player.h"
#include "j1Colliders.h"
#include "j1Fade2Black.h"
#include "j1Scene.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;
	notfirst = false;
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	hasExit = false;
	if (lvl1)
	{
		App->tiles->Load("maps/Level1.tmx");
	}
	if (lvl2)
	{
		App->tiles->Load("maps/Level2.tmx");
	}
	if (notfirst)
	{
		App->player->Load("animations/Player.tmx");
	}
	
	return true;
}


// Called each loop iteration
bool j1Scene::PreUpdate()
{

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		App->render->camera.y += 4;
		App->tiles->culling_Collider->rect.y -= 2;
		App->tiles->camera_Collider->rect.y -= 2;
	}
	

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		App->render->camera.y -= 4;
		App->tiles->culling_Collider->rect.y += 2;
		App->tiles->camera_Collider->rect.y += 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {

			App->render->camera.x += 4;
			App->tiles->culling_Collider->rect.x -= 2;
			App->tiles->camera_Collider->rect.x -= 2;
		
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			App->render->camera.x -= 4;
			App->tiles->culling_Collider->rect.x += 2;
			App->tiles->camera_Collider->rect.x += 2;
		

	}

	if (App->player->GetPlayerCollider()->GetPosition().x > camera_limit_right + App->tiles->camera_Collider->rect.w) {
		if (App->scene->lvl1 == true)
		{
			App->scene->lvl2 = true;
			App->scene->lvl1 = false;
		}
		else if (App->scene->lvl2 == true)
		{
			App->scene->lvl1 = true;
			App->scene->lvl2 = false;
		}
		App->fade2black->FadeToBlack(App->scene, App->scene);
	}

	if (hasExit) {
		App->fade2black->FadeToBlack(App->scene, App->scene);
		App->scene->lvl2 = false;
		App->scene->lvl1 = true;
	}
		App->tiles->Draw();
	
	
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->tiles->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->tiles->map_Data.width, App->tiles->map_Data.height,
					App->tiles->map_Data.tile_width, App->tiles->map_Data.tile_height,
					App->tiles->map_Data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->tiles->CleanUp();
	App->collider->CleanUp();
	App->player->CleanUp();	
	notfirst = true;
	return true;
}

bool j1Scene::LoadSceneLimits(pugi::xml_node object)
{
	for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
	{
		if (strcmp(it.attribute("name").as_string(), "Limit Left") == 0)
		{
			camera_limit_left = it.attribute("value").as_int();
		}
		if (strcmp(it.attribute("name").as_string(), "Limit Right") == 0)
		{
			camera_limit_right = it.attribute("value").as_int();
		}
	}

		return true;
}

bool j1Scene::SaveGame(pugi::xml_node save_game)
{

	return true;
}


