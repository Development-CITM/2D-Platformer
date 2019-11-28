#include "p2Log.h"
#include "j1EntityManager.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "PlayerObjects.h"
#include "EnemiesObjects.h"
#include "j1Scene.h"




j1EntityManager::j1EntityManager()
{
}

j1EntityManager::~j1EntityManager()
{}

bool j1EntityManager::Awake(pugi::xml_node & conf)
{
	//Load all textures,sfx here? so we just need to print them not loading each time
	bool ret = true;
	return ret;
}

bool j1EntityManager::Start()
{
	bool ret = true;
	CreatePlayer();
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

	DestroyEnemies();
	
	return ret;
}

void j1EntityManager::CreatePlayer()
{
	/*Object_Player* player = new Object_Player(1);
	objects.add(player);*/
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

void j1EntityManager::CreateEnemy(p2Point<int> pos, Object_type type)
{
	if (App->scene->loading == false)
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
	else
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

	for (int i = backup.count() - 1; i >= 0; i--)
	{	
			backup.At(i)->data->CleanUp();
			backup.del(backup.At(i));
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

