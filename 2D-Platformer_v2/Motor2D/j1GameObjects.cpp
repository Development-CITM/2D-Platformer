#include "j1GameObjects.h"
#include "DynamicObjects.h"


j1GameObjects::j1GameObjects()
{}

j1GameObjects::~j1GameObjects()
{}

bool j1GameObjects::Awake(pugi::xml_node & conf)
{
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
	return ret;
}

bool j1GameObjects::Update(float dt)
{
	bool ret = true;
	return ret;
}

bool j1GameObjects::PostUpdate()
{
	bool ret = true;
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

