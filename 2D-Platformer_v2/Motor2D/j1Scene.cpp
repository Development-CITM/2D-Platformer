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
#include "j1Debug.h"
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
	hasExit = false;
	if (notfirst)
	{
		App->player->Load("animations/Player.tmx");
	}

	DecideMapToLoad();
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
	if (App->debug->input == true)
	{
		if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
			App->LoadGame("save_game.xml");

		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
			App->SaveGame("save_game.xml");

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
			App->render->camera.y += 4;
			App->tiles->culling_Collider->rect.y -= 2;

		}
	}
	
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		App->render->camera.y -= 4;
		App->tiles->culling_Collider->rect.y += 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {

			App->render->camera.x += 4;
			App->tiles->culling_Collider->rect.x -= 2;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
			App->render->camera.x -= 4;
			App->tiles->culling_Collider->rect.x += 2;
	}

	//if (App->player->GetPlayerCollider()->GetPosition().x > camera_limit_right + App->tiles->culling_Collider->rect.w) {
	//	if (App->scene->lvl1 == true)
	//	{
	//		App->scene->lvl2 = true;
	//		App->scene->lvl1 = false;
	//	}
	//	else if (App->scene->lvl2 == true)
	//	{
	//		App->scene->lvl1 = true;
	//		App->scene->lvl2 = false;
	//	}
	//	App->fade2black->FadeToBlack(App->scene, App->scene);
	//}

	if (hasExit) {
		App->fade2black->FadeToBlack(App->scene, App->scene);
		/*App->scene->lvl2 = false;
		App->scene->lvl1 = true;*/
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

#pragma region Load/Save
// Load Game State
bool j1Scene::Load(pugi::xml_node& data)
{
		loading = true;
		App->fade2black->FadeToBlack(App->scene, App->scene);

		App->tiles->culling_pos_x = data.child("culling").attribute("x").as_int();
		App->tiles->culling_pos_y = data.child("culling").attribute("y").as_int();
	
		App->render->camera.x = data.child("camera").attribute("x").as_int();
		App->render->camera.y = data.child("camera").attribute("y").as_int();

		App->player->mPosition.x = data.child("playerPos").attribute("player_pos_x").as_float();
		App->player->mPosition.y = data.child("playerPos").attribute("player_pos_y").as_float();
		App->player->groundPos = data.child("playerPos").attribute("ground_pos").as_int();

		destination_level = data.child("scene").attribute("current_map").as_string();
		current_level = data.child("scene").attribute("current_map").as_string();

		

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	//------------PLAYER-------------------//
	pugi::xml_node player = data.append_child("playerPos");

	player.append_attribute("player_pos_x") = App->player->mPosition.x;
	player.append_attribute("player_pos_y") = App->player->mPosition.y;
	player.append_attribute("ground_pos") = App->player->groundPos; //EUDALD: DELETE WHEN COLLIDERS WORKING

	pugi::xml_node player_collider = data.append_child("player_collider");

	//player_collider=data.append_child("") EUDALD: SAVE/LOAD PLAYER POS(SAVEGAME) FLOAT, 



	//------------CAMERA --------------------//
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = App->render->camera.x;
	cam.append_attribute("y") = App->render->camera.y;

	pugi::xml_node cull = data.append_child("culling");

	cull.append_attribute("x") = App->tiles->culling_Collider->rect.x;
	cull.append_attribute("y") = App->tiles->culling_Collider->rect.y;

	pugi::xml_node current_lvl = data.append_child("current_lvl");

	//current_lvl.append_attribute("lvl1") = lvl1;
	//current_lvl.append_attribute("lvl2") = lvl2;

	//-------------SCENE-------------------//
	pugi::xml_node scene = data.append_child("scene");

	scene.append_attribute("current_map") = current_level.GetString();

	return true;
}
#pragma endregion

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	p2List_item<p2SString*>* item = levels.start;

	levels.clear();
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


void j1Scene::DecideMapToLoad()
{
	levels.add(&A1);
	levels.add(&A2);
	levels.add(&A3);
	levels.add(&A5);
	levels.add(&A6);

	//p2Lis
	p2List_item<p2SString*>* lvl= levels.start;
	p2SString* level = lvl->data;
	for (int i = 0; i < levels.count(); i++)
	{
		if (strcmp(level->GetString(), destination_level.GetString()) == 0)
		{
			App->tiles->Load(level->GetString());
			current_level = level->GetString();
			break;
		}

		if(lvl->next != nullptr)
		{
			lvl = lvl->next;
			level = lvl->data;
		}
	}
}






