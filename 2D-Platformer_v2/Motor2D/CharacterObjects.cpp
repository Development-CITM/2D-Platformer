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

void Object_Character::CreatePlayer()
{
	Object_Player* player_object = new Object_Player();
	character_objects.add(player_object);
}
