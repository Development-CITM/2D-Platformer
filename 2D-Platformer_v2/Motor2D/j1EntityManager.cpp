#include "j1EntityManager.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "PlayerObjects.h"
#include "EnemiesObjects.h"




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
	CreateEnemyGround();
	p2List_item<GameObject*>* item;
	//objects.clear();
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
	return ret;
}

void j1EntityManager::CreatePlayer()
{
	Object_Player* player = new Object_Player(1);
	objects.add(player);
}

void j1EntityManager::CreateEnemyGround()
{
	Object_Enemy* enemy_ground = new Object_Enemy(Object_type::ENEMY_GROUND, { 500,400 });
	objects.add(enemy_ground);

	Object_Enemy* enemy_fly = new Object_Enemy(Object_type::ENEMY_FLYING, {700,400});
	objects.add(enemy_fly);
}

void j1EntityManager::CreateEnemyFlying()
{
	//Enemy flying creation
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

