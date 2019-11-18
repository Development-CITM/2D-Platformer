#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "p2Defs.h"
#include "p2Log.h"
Object_Dynamic::Object_Dynamic()
{
}
Object_Dynamic::Object_Dynamic(Dynamic_type type): EntityManager()
{
	
	if (type == Dynamic_type::CHARACTER)
	{
		CreateCharacter();
	}
}

Object_Dynamic::~Object_Dynamic()
{
}

bool Object_Dynamic::PreUpdate()
{
	bool ret = true;

	p2List_item<Object_Character*>* item;
	item = character_objects.start;

	while (item != NULL && ret == true)
	{
		item->data->PreUpdate();
		item = item->next;
  	}
	return ret;
}

bool Object_Dynamic::Update()
{
	bool ret = true;

	p2List_item<Object_Character*>* item;
	item = character_objects.start;

	while (item != NULL && ret == true)
	{
		item->data->Update();
		item = item->next;
	}
	return ret;
}

bool Object_Dynamic::PostUpdate()
{
	bool ret = true;

	p2List_item<Object_Character*>* item;
	item = character_objects.start;

	while (item != NULL && ret == true)
	{
		item->data->PostUpdate();
		item = item->next;
	}
	return ret;
}

void Object_Dynamic::CreateCharacter()
{
	Object_Character* character_object = new Object_Character(Character_type::PLAYER);
	character_objects.add(character_object);
}
