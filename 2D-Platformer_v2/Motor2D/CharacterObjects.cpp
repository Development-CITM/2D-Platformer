#include "j1GameObjects.h"
#include "CharacterObjects.h"
#include "DynamicObjects.h"
#include "PlayerObjects.h"
#include "p2Defs.h"
#include "p2Log.h"

Object_Character::Object_Character()
{
}

Object_Character::Object_Character(Character_type type):Object_Dynamic()
{
	if (type == Character_type::PLAYER)
	{
		CreatePlayer();
	}
}

Object_Character::~Object_Character()
{
}

bool Object_Character::PreUpdate()
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

bool Object_Character::Update()
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

bool Object_Character::PostUpdate()
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

void Object_Character::CreatePlayer()
{
	player_object = new Object_Player(1);
	character_objects.add(player_object);
}
