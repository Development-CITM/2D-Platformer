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

bool Object_Dynamic::Preupdate()
{
	bool ret = true;
	return ret;
}

void Object_Dynamic::CreateCharacter()
{
	Object_Character* character_object = new Object_Character(Character_type::PLAYER);
	dynamic_objects.add(character_object);
}
