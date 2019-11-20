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
#include "j1PathFinding.h"
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

	debug_tex = App->tex->Load("maps/path2.png");
	DecideMapToLoad();
	return true;
}


// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->tiles->WorldToMap(p.x, p.y);
	


	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = App->tiles->WorldToMap(App->player->GetCollider()->GetPosition().x, App->player->GetCollider()->GetPosition().y);
			origin_selected = true;
		}
	}
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

	/*iPoint map_coordinates = App->tiles->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->tiles->map_Data.width, App->tiles->map_Data.height,
					App->tiles->map_Data.tile_width, App->tiles->map_Data.tile_height,
					App->tiles->map_Data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());*/
	// Debug pathfinding ------------------------------
//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->tiles->WorldToMap(p.x, p.y);
	p = App->tiles->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x , p.y,NULL,App->render->drawsize);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->tiles->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y,NULL, App->render->drawsize);
	}
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

		App->player->playerPos.x = data.child("playerPos").attribute("player_pos_x").as_float();
		App->player->playerPos.y = data.child("playerPos").attribute("player_pos_y").as_float();

		destination_level = data.child("scene").attribute("current_map").as_string();
		current_level = data.child("scene").attribute("current_map").as_string();

		

	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	//------------PLAYER-------------------//
	pugi::xml_node player = data.append_child("playerPos");

	player.append_attribute("player_pos_x") = App->player->playerPos.x;
	player.append_attribute("player_pos_y") = App->player->playerPos.y;

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
			if (App->tiles->Load(level->GetString()) == true)
			{
				int w, h;
				uchar* data = NULL;
				if (App->tiles->CreateWalkabilityMap(w, h, &data))
					App->pathfinding->SetMap(w, h, data);

				RELEASE_ARRAY(data);
			}
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

void j1Scene::ColliderMapToLoad(Collider* c2)
{
	if (c2->swap==A2_TO_A1)
	{
		destination_level = "maps/A1.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A1_TO_A2)
	{
		destination_level = "maps/A2.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A5_TO_A1)
	{
		destination_level = "maps/A1.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A1_TO_A5)
	{
		destination_level = "maps/A5.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A3_TO_A2)
	{
		destination_level = "maps/A2.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A2_TO_A3)
	{
		destination_level = "maps/A3.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}
	else if (c2->swap == A5_TO_A6)
	{
		destination_level = "maps/A6.tmx";
		App->debug->CallFade();
		c2->Enabled = true;
	}

	swapping = true;
}






