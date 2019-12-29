#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Fade2Black.h"
#include "j1PathFinding.h"
#include "j1Debug.h"
#include "j1UI.h"
#include "j1EntityManager.h"
#include "UI_Functions.h"
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
	debug_tex = App->tex->Load("maps/path2.png");
	DecideMapToLoad();
	if (strcmp(current_level.GetString(), "maps/A5.tmx") != 0)
	{
		App->ui->timer.Start();
	}

	if (notfirst)
	{
		App->entity->Start();
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
	if (App->debug->input == true)
	{
		if (App->scene->saving)
		{
			if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
			{
				App->debug->CallFade();
				App->LoadGame("save_game.xml");
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			App->scene->saving = true;
			App->SaveGame("save_game.xml");
		}	

	}
		App->tiles->Draw();
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && strcmp(current_level.GetString(),"maps/A5.tmx")!=0)
	{
		if (!pause)
		{
			UI_Functions::ShowPauseMenu(App->ui->pausemenuBackground);
			App->pause = true;
		}
		else
		{
			App->ui->pausemenuBackground->ToggleHide(true);
			App->pause = false;
		}
		pause = !pause;
	}

	return ret;
}

#pragma region Load/Save
// Load Game State
bool j1Scene::Load(pugi::xml_node& data)
{
		loading = true;
		//App->fade2black->FadeToBlack(App->scene, App->scene);

		//----------------CULLING--------------//
		App->tiles->culling_pos_x = data.child("culling").attribute("x").as_int();
		App->tiles->culling_pos_y = data.child("culling").attribute("y").as_int();

		//---------------CAMERA--------------//
		App->render->camera.x = data.child("camera").attribute("x").as_int();
		App->render->camera.y = data.child("camera").attribute("y").as_int();

		//--------------PLAYER--------------//
		player_backup_x = data.child("playerPos").attribute("player_pos_x").as_float();
		player_backup_y = data.child("playerPos").attribute("player_pos_y").as_float();
		player_backup_collider_x = data.child("playerPos").attribute("player_collider_pos_x").as_float();
		player_backup_collider_y = data.child("playerPos").attribute("player_collider_pos_y").as_float();

		//-------------LEVEL SELECTION----------------//
		destination_level = data.child("scene").attribute("current_map").as_string();
		current_level = data.child("scene").attribute("current_map").as_string();

		//-------------ENEMIES-----------------------//
		
		int type_id;
		int id = 1;
		p2SString enemy_identificator;
		p2SString enemy = "enemy_";
		p2SString x = "_x";
		p2SString y = "_y";
		p2SString type_string = "_type";

		p2List_item<GameObject*>* it = App->entity->backup.start;

		for(int i=0;i< App->entity->backup.count();i++)
		{
			enemy_identificator = p2SString("%d", id);

			enemy += enemy_identificator;
			App->entity->backup.At(i)->data->position.x = data.child("enemies").attribute((enemy += x).GetString()).as_int();
			enemy = "enemy_";

			enemy += enemy_identificator;
			App->entity->backup.At(i)->data->position.y = data.child("enemies").attribute((enemy += y).GetString()).as_int();
			enemy = "enemy_";
			id++;
		}	
	
	return true;
}

// Save Game State
bool j1Scene::Save(pugi::xml_node& data) const
{
	p2List_item<GameObject*>* item;
	//------------PLAYER-------------------//
	pugi::xml_node player = data.append_child("playerPos");

	item = App->entity->objects.start;
	while (item != NULL)
	{
		if (item->data->type_object == Object_type::PLAYER)
		{
			player.append_attribute("player_pos_x") = item->data->position.x;
			player.append_attribute("player_pos_y") = item->data->position.y;
			player.append_attribute("player_collider_pos_x") = item->data->collider->rect.x;
			player.append_attribute("player_collider_pos_y") = item->data->collider->rect.y;
		}
		item = item->next;
	}
	
	//------------CAMERA --------------------//
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = App->render->camera.x;
	cam.append_attribute("y") = App->render->camera.y;

	pugi::xml_node cull = data.append_child("culling");

	cull.append_attribute("x") = App->tiles->culling_Collider->rect.x;
	cull.append_attribute("y") = App->tiles->culling_Collider->rect.y;

	pugi::xml_node current_lvl = data.append_child("current_lvl");
	//-------------SCENE-------------------//
	pugi::xml_node scene = data.append_child("scene");

	scene.append_attribute("current_map") = current_level.GetString();

	//-------------ENEMIES---------------//
	App->entity->ClearBackup();
	pugi::xml_node enemies = data.append_child("enemies");

	item = App->entity->objects.start;
	int id = 1;
	int type_id;
	p2SString enemy_identificator;
	p2SString enemy = "enemy_";
	p2SString x = "_x";
	p2SString y = "_y";
	p2SString type_string = "_type";
	while (item != NULL)
	{	
		if (item->data->type_object == Object_type::ENEMY_FLYING || item->data->type_object == Object_type::ENEMY_GROUND)
		{
				enemy_identificator = p2SString("%d", id);

				enemy += enemy_identificator;
				enemies.append_attribute((enemy += x).GetString()) = item->data->position.x;
				enemy = "enemy_";

				enemy += enemy_identificator;
				enemies.append_attribute((enemy += y).GetString()) = item->data->position.y;
				enemy = "enemy_";

				enemy += enemy_identificator;
				enemies.append_attribute((enemy += type_string).GetString()) = item->data->type_object;
				enemy = "enemy_";

				App->entity->FillBackup({ item->data->position.x,item->data->position.y }, item->data->type_object);
				id++;
		}
		item = item->next;
	}
	//----------COINS-----------------//
	App->entity->ClearCoinBackup();
	pugi::xml_node coins = data.append_child("coins");
	item = App->entity->objects.start;
	id = 1;
	p2SString coin_id;
	p2SString coin = "coin_";
	p2SString collider = "_collider";
	while (item != NULL)
	{
		if (item->data->type_object == Object_type::COIN)
		{
			coin_id = p2SString("%d", id);

			coin += coin_id;
			coins.append_attribute((coin += x).GetString()) = item->data->position.x;
			coins.append_attribute((coin += collider).GetString()) = item->data->collider->rect.x;
			coin = "coin_";

			coin += coin_id;
			coins.append_attribute((coin += y).GetString()) = item->data->position.y;
			coins.append_attribute((coin += collider).GetString()) = item->data->collider->rect.y;
			coin = "coin_";

			App->entity->FillCoinBackup({ item->data->position.x,item->data->position.y }, { item->data->collider->rect.x, item->data->collider->rect.y});
			id++;
		}
		item = item->next;
	}


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
	App->entity->CleanUp();
	notfirst = true;
	return true;
}

bool j1Scene::LoadSceneLimits(pugi::xml_node object)
{
	for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
	{
		if (strcmp(it.attribute("name").as_string(), "limit_left") == 0)
		{
			camera_limit_left = it.attribute("value").as_int();
		}
		else if (strcmp(it.attribute("name").as_string(), "limit_right") == 0)
		{
			camera_limit_right = it.attribute("value").as_int();
		}
		else if (strcmp(it.attribute("name").as_string(), "limit_bot") == 0)
		{
			camera_limit_bot = it.attribute("value").as_int();
		}
		else if (strcmp(it.attribute("name").as_string(), "limit_top") == 0)
		{
			camera_limit_top = it.attribute("value").as_int();
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
			/*if (strcmp(current_level.GetString(), "maps / A5.tmx") == 0)
			{
				UI_Functions::ShowUI(App->ui->backgroundImage, App->ui->continueButton, App->ui->creditsButton, App->ui->playButton)
			}*/
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






