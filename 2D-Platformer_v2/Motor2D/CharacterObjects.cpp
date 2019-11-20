#include "j1GameObjects.h"
#include "CharacterObjects.h"
#include "DynamicObjects.h"
#include "PlayerObjects.h"
#include "j1App.h"
#include "j1Textures.h"
#include "p2Defs.h"
#include "p2Log.h"



Object_Character::Object_Character():Object_Dynamic()
{

}

Object_Character::~Object_Character()
{
}

bool Object_Character::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool Object_Character::Update(float dt)
{
		bool ret = true;

		return ret;
}

bool Object_Character::PostUpdate()
{
	bool ret = true;

	return ret;
}

bool Object_Character::Load(const char* file_name)
{
	bool ret = true;
	pugi::xml_document	player_file;
	pugi::xml_parse_result result = player_file.load_file(file_name);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	//Load info
	if (ret == true) {

		pugi::xml_node	player_node = player_file.child("map");

		LoadCharacterTMX(player_node);

		pugi::xml_node pre_group = player_node.child("group");
		pugi::xml_node group = pre_group.child("group");

		for (group; group && ret; group = group.next_sibling("group"))
		{
			pugi::xml_node object_group = group.child("objectgroup");
			LoadAnimation(object_group);
		}
	}

	return ret;
}

bool Object_Character::LoadCharacterTMX(pugi::xml_node& character_node)
{
	bool ret = true;
	if (character_node == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		character_tmx_data.width = character_node.attribute("width").as_int();
		character_tmx_data.height = character_node.attribute("height").as_int();
		character_tmx_data.tile_width = character_node.attribute("tilewidth").as_int();
		character_tmx_data.tile_height = character_node.attribute("tileheight").as_int();

		p2SString image_source("animations/");
		image_source += character_node.child("imagelayer").child("image").attribute("source").as_string();
		character_tmx_data.texture = App->tex->Load(image_source.GetString());
		if (character_tmx_data.texture) {
			LOG("Player Texture succesfull loaded");
		}
		else {
			LOG("Error loading player texture");
			ret = false;
		}
	}
	return ret;
}

Animation* Object_Character::LoadAnimation(pugi::xml_node& obj_group)
{
	return nullptr;
}

