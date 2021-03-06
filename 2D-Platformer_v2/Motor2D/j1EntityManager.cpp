#include "p2Log.h"
#include "j1EntityManager.h"
#include "CharacterObjects.h"
#include "PlayerObjects.h"
#include "EnemiesObjects.h"
#include "CoinObjects.h"
#include "j1Scene.h"
#include "j1Audio.h"
#include "j1Audio.h"
#include "j1UI.h"
#include "j1Textures.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1EntityManager::j1EntityManager()
{
}

j1EntityManager::~j1EntityManager()
{}

bool j1EntityManager::Awake(pugi::xml_node & conf)
{
	bool ret = true;
	return ret;
}

bool j1EntityManager::Start()
{
	App->audio->LoadFx("audio/fx/jump_fx.wav");
	App->audio->LoadFx("audio/fx/double_jump_fx.wav");
	App->audio->LoadFx("audio/fx/water_sfx.wav");
	App->audio->LoadFx("audio/fx/dead_fx.wav");
	App->audio->LoadFx("audio/fx/punch.wav");
	App->audio->LoadFx("audio/fx/kick.wav");
	App->audio->LoadFx("audio/fx/whisp_die.wav");
	App->audio->LoadFx("audio/fx/kobold_die.wav");
	App->audio->LoadFx("audio/fx/coin.wav");
	App->audio->LoadFx("audio/UI/hover.wav");
	App->audio->LoadFx("audio/UI/click.wav");
	App->audio->LoadFx("audio/UI/pause.wav");

	bool ret = true;
	p2List_item<GameObject*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	
	return ret;
}

bool j1EntityManager::PreUpdate()
{
	bool ret = true;
	p2List_item<GameObject*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->PreUpdate();
		item = item->next;
	}
	return ret;
}

bool j1EntityManager::Update(float dt)
{
	bool ret = true;
	p2List_item<GameObject*>* item;
	item = objects.start;
	while (item != NULL && ret == true)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}
	DestroyEnemyDead();
	DestroyPickedCoins();
	return ret;
}

bool j1EntityManager::PostUpdate()
{
	bool ret = true;
	p2List_item<GameObject*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->PostUpdate();
		item = item->next;
	}
	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	p2List_item<Mix_Chunk*>* item = App->audio->fx.start;
	{
		for (int i = App->audio->fx.count() - 1; i >= 0; i--)
		{
			Mix_FreeChunk(item->data);
			App->audio->fx.del(App->audio->fx.At(i));
		}
	}
	DestroyEnemies();
	DestroyPlayer();
	DestroyCoin();
	
	return ret;
}


void j1EntityManager::DestroyEnemies()
{
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if (objects.At(i)->data->type_object == Object_type::ENEMY_GROUND || objects.At(i)->data->type_object == Object_type::ENEMY_FLYING)
		{
			objects.At(i)->data->CleanUp();
			objects.del(objects.At(i));
		}
	}				
}

void j1EntityManager::DestroyEnemyDead()
{
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if ((objects.At(i)->data->type_object == Object_type::ENEMY_GROUND || objects.At(i)->data->type_object == Object_type::ENEMY_FLYING) && objects.At(i)->data->alive==false)
		{
			objects.At(i)->data->CleanUp();
			objects.del(objects.At(i));
		}
	}
}

void j1EntityManager::DestroyPickedCoins()
{
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if (objects.At(i)->data->type_object == Object_type::COIN && objects.At(i)->data->alive == false)
		{
			objects.At(i)->data->CleanUp();
			objects.del(objects.At(i));
		}
	}
}

void j1EntityManager::DestroyPlayer()
{
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if (objects.At(i)->data->type_object == Object_type::PLAYER)
		{
			objects.At(i)->data->CleanUp();
			objects.del(objects.At(i));
		}
	}
}

void j1EntityManager::DestroyCoin()
{
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if (objects.At(i)->data->type_object == Object_type::COIN)
		{
			objects.At(i)->data->CleanUp();
			objects.del(objects.At(i));
		}
	}
}

void j1EntityManager::CreateCoin(pugi::xml_node& object)
{
	
	Object_Coin* coin = new Object_Coin(object);
	objects.add(coin);
}

void j1EntityManager::CreateEnemy(p2Point<int> pos, Object_type type)
{
		if (type == Object_type::ENEMY_GROUND)
		{
			Object_Enemy* enemy_ground = new Object_Enemy(type, pos);
			objects.add(enemy_ground);
		}
		else if (type == Object_type::ENEMY_FLYING)
		{
			Object_Enemy* enemy_fly = new Object_Enemy(type, pos);
			objects.add(enemy_fly);
		}
}

void j1EntityManager::FillBackup(p2Point<int> pos, Object_type type)
{
	if (type == Object_type::ENEMY_GROUND)
	{
		Object_Enemy* enemy_ground = new Object_Enemy(type, pos);
		backup.add(enemy_ground);
	}
	else if (type == Object_type::ENEMY_FLYING)
	{
		Object_Enemy* enemy_fly = new Object_Enemy(type, pos);
		backup.add(enemy_fly);
	}
}

void j1EntityManager::FillCoinBackup(p2Point<int> pos, p2Point<int> collider)
{
	Object_Coin* coin = new Object_Coin(pos,collider);
	coin_backup.add(coin);
}

void j1EntityManager::LoadEnemiesFromMap(pugi::xml_node& object)
{
		Object_type type;
		p2Point<int> pos;
		for (pugi::xml_node all = object; all; all = all.next_sibling("object"))
		{
			if (strcmp(all.attribute("name").as_string(), "Kobold") == 0)
			{
				type = Object_type::ENEMY_GROUND;
				for (pugi::xml_node it = all.child("properties").child("property"); it; it = it.next_sibling("property"))
				{
					if (strcmp(it.attribute("name").as_string(), "kobold_pos_x") == 0)
					{
						pos.x = it.attribute("value").as_int();
					}
					else if (strcmp(it.attribute("name").as_string(), "kobold_pos_y") == 0)
					{
						pos.y = it.attribute("value").as_int();
					}
				}
				CreateEnemy(pos, type);
			}
			else if (strcmp(all.attribute("name").as_string(), "Whisp") == 0)
			{
				type = Object_type::ENEMY_FLYING;
				for (pugi::xml_node it = all.child("properties").child("property"); it; it = it.next_sibling("property"))
				{
					if (strcmp(it.attribute("name").as_string(), "whisp_pos_x") == 0)
					{
						pos.x = it.attribute("value").as_int();
					}
					else if (strcmp(it.attribute("name").as_string(), "whisp_pos_y") == 0)
					{
						pos.y = it.attribute("value").as_int();
					}
				}
				CreateEnemy(pos, type);
			}	
		}
}

void j1EntityManager::LoadEnemiesFromBackup()
{
	p2List_item<GameObject*>* it_objects = objects.start;
	p2List_item<GameObject*>* it_backups = backup.start;
	
	while (it_backups != NULL)
	{
		objects.add(it_backups->data);
		it_backups = it_backups->next;
	}
}

void j1EntityManager::LoadCoinsFromBackup()
{
	p2List_item<GameObject*>* it_objects = objects.start;
	p2List_item<GameObject*>* it_backups = coin_backup.start;

	while (it_backups != NULL)
	{
		objects.add(it_backups->data);
		it_backups = it_backups->next;
	}
}

void j1EntityManager::CreatePlayer(pugi::xml_node& object)
{
	Object_Player* player = new Object_Player(object);
	objects.add(player);
}

void j1EntityManager::KillPlayerTimeOff()
{
	bool just_once = true;
	for (int i = objects.count() - 1; i >= 0; i--)
	{
		if (objects.At(i)->data->type_object == Object_type::PLAYER)
		{
			if (just_once)
			{
				App->audio->PlayFx(4);
				just_once = false;
			}
			objects.At(i)->data->alive = false;
		}
	}
}

Collider* j1EntityManager::RetreivePlayerCollider()
{
	p2List_item<GameObject*>* item;
	Collider* ret = nullptr;
	item = objects.start;

	while (item != NULL)
	{
		if (item->data->type_object == Object_type::PLAYER)
		{
			ret = item->data->GetCollider();
		}
		item = item->next;
	}
	return ret;
}

CharacterTMXData j1EntityManager::RetreivePlayerData()
{
	CharacterTMXData tmx;
	p2List_item<GameObject*>* item;
	item = objects.start;

	while (item != NULL)
	{
		if (item->data->type_object == Object_type::PLAYER)
		{
			tmx = item->data->character_tmx_data;
		}
		item = item->next;
	}
	return tmx;
}

void j1EntityManager::ClearBackup()
{
	for (int i = backup.count() - 1; i >= 0; i--)
	{
		backup.At(i)->data->CleanUp();
		backup.del(backup.At(i));
	}
}

void j1EntityManager::ClearCoinBackup()
{
	for (int i = coin_backup.count() - 1; i >= 0; i--)
	{
		coin_backup.At(i)->data->CleanUp();
		coin_backup.del(coin_backup.At(i));
	}
}

//ENTITY MANAGER ----------------------------------------------------------------------------------------------------------------------------------------

bool GameObject::Start()
{
	bool ret = true;
	return ret;
}

bool GameObject::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;
	return ret;
}

bool GameObject::PostUpdate()
{
	bool ret = true;
	return ret;
}

bool GameObject::CleanUp()
{
	return false;
}

