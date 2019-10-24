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
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if (map % 2 == 0)
	{
		App->tiles->Load("maps/Level1.tmx");
	}
	if (map % 2 != 0)
	{
		App->tiles->Load("maps/Level2.tmx");
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
	}
	

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		App->render->camera.y -= 4;
		App->tiles->culling_Collider->rect.y += 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		if (App->render->camera.x + 3 < -limitleft.rect.w) {
			App->render->camera.x += 4;
			App->tiles->culling_Collider->rect.x -= 2;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		if (App->render->camera.x - App->win->GetWidth() - 3 > -limitright.rect.x * (int)App->render->drawsize) {
			App->render->camera.x -= 4;
			App->tiles->culling_Collider->rect.x += 2;
		}

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
	//App->player->CleanUp();
	map++;
	return true;
}

bool j1Scene::LoadSceneLimits(pugi::xml_node object)
{
	Cameralimit* limit = new Cameralimit();
	for (object.child("object"); object; object = object.next_sibling("object"))
	{
		if (strcmp(object.attribute("type").as_string(), "Right") == 0)
		{
			limitright.rect.x = object.attribute("x").as_int();
			limitright.rect.y = object.attribute("y").as_int();
			limitright.rect.w = object.attribute("width").as_int();
			limitright.rect.h = object.attribute("height").as_int();
		}
		else if (strcmp(object.attribute("type").as_string(), "Left") == 0)
		{
			limitleft.rect.x = object.attribute("x").as_int();
			limitleft.rect.y = object.attribute("y").as_int();
			limitleft.rect.w = object.attribute("width").as_int();
			limitleft.rect.h = object.attribute("height").as_int();
		}
	}

		return true;
}


