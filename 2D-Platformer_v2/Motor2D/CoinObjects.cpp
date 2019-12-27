#include "CoinObjects.h"
#include "j1EntityManager.h"
#include "CharacterObjects.h"
#include "j1Colliders.h"
#include "PlayerObjects.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "p2Defs.h"
#include "p2Log.h"

Object_Coin::Object_Coin(pugi::xml_node& object) : Object_Character()
{
	SetPos(object);
	collider = App->collider->AddCollider({ position.x+13,position.y+13,20,25 }, COLLIDER_COIN);
}

Object_Coin::~Object_Coin()
{
}

bool Object_Coin::Start()
{
	bool ret = true;
	Load("animations/Coin.tmx");
	alive = true;
	type_object = COIN;
	currentAnimation = spin;
	return ret;
}

bool Object_Coin::PreUpdate()
{
	return true;
}

bool Object_Coin::Update(float dt)
{
		if (!collider->Enabled)
			alive = false;
		else
			Draw(dt);

	return true;
}

bool Object_Coin::PostUpdate()
{
	return true;
}


Animation* Object_Coin::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "SPIN") == 0) { spin = anim; spin->loop = true; }

	anim->num_sprites = obj_group.child("properties").child("property").last_attribute().as_int();

	anim->sprites = new Sprite[anim->num_sprites];
	int i = 0;
	pugi::xml_node AABB_object = obj_group.next_sibling("objectgroup").child("object");

	for (pugi::xml_node object = obj_group.child("object"); object; object = object.next_sibling("object"))
	{
		anim->sprites[i].rect.x = object.attribute("x").as_int();
		anim->sprites[i].rect.y = object.attribute("y").as_int();
		anim->sprites[i].rect.w = object.attribute("width").as_int();
		anim->sprites[i].rect.h = object.attribute("height").as_int();

		anim->sprites[i].frames = object.child("properties").child("property").attribute("value").as_int();
		i++;
	}
	return anim;
}


void Object_Coin::SetPos(pugi::xml_node& object)
{
	for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
	{
		if (strcmp(it.attribute("name").as_string(), "coin_pos_x") == 0)
		{
			position.x = it.attribute("value").as_int();
		/*	if (collider != nullptr)
			{
				collider->rect.x = it.attribute("value").as_int();
			}*/

		}
		if (strcmp(it.attribute("name").as_string(), "coin_pos_y") == 0)
		{
			position.y = it.attribute("value").as_int();
	/*		if (collider != nullptr)
			{
				collider->rect.y = it.attribute("value").as_int();
			}*/
		}
	}
}
