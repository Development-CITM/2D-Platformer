#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "PlayerObjects.h"
#include "EnemiesObjects.h"




j1GameObjects::j1GameObjects()
{}

j1GameObjects::~j1GameObjects()
{}

bool j1GameObjects::Awake(pugi::xml_node & conf)
{
	//Load all textures,sfx here? so we just need to print them not loading each time
	bool ret = true;
	return ret;
}

bool j1GameObjects::Start()
{
	bool ret = true;
	CreatePlayer();
	CreateEnemyGround();
	p2List_item<EntityManager*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	return ret;

	return ret;
}

bool j1GameObjects::PreUpdate()
{
	bool ret = true;
	p2List_item<EntityManager*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->PreUpdate();
		item = item->next;
	}
	return ret;
}

bool j1GameObjects::Update(float dt)
{
	bool ret = true;
	p2List_item<EntityManager*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}
	return ret;
}

bool j1GameObjects::PostUpdate()
{
	bool ret = true;
	p2List_item<EntityManager*>* item;
	item = objects.start;

	while (item != NULL && ret == true)
	{
		ret = item->data->PostUpdate();
		item = item->next;
	}
	return ret;
}

bool j1GameObjects::CleanUp()
{
	bool ret = true;
	return ret;
}

void j1GameObjects::CreatePlayer()
{
	Object_Player* player = new Object_Player(1);
	objects.add(player);
}

void j1GameObjects::CreateEnemyGround()
{
	Object_Enemy* enemy_ground = new Object_Enemy(Object_type::ENEMY_GROUND);
	//Prove that we can access through different instances of the same class
	Object_Enemy* enemy_fly = new Object_Enemy(Object_type::ENEMY_FLYING);
	objects.add(enemy_ground);
	objects.add(enemy_fly);
}

void j1GameObjects::CreateEnemyFlying()
{
	//Enemy flying creation
}

//ENTITY MANAGER ----------------------------------------------------------------------------------------------------------------------------------------

bool EntityManager::Start()
{
	bool ret = true;
	return ret;
}

bool EntityManager::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	return ret;
}

bool EntityManager::PostUpdate()
{
	bool ret = true;
	return ret;
}

