#include "j1GameObjects.h"
#include "DynamicObjects.h"


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
	CreateDynamicObject();
	bool ret = true;
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
		ret = item->data->Update();
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

void j1GameObjects::CreateDynamicObject()
{
	Object_Dynamic* dynamic_object = new Object_Dynamic(Dynamic_type::CHARACTER);	 
	objects.add(dynamic_object);
}


//ENTITY MANAGER ----------------------------------------------------------------------------------------------------------------------------------------

bool EntityManager::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool EntityManager::Update()
{
	bool ret = true;
	return ret;
}

bool EntityManager::PostUpdate()
{
	bool ret = true;
	return ret;
}

