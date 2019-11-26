#include "EnemiesObjects.h"
#include "p2Log.h"
#include "j1Colliders.h"

Object_Enemy::Object_Enemy(Object_type type, p2Point<int> pos) : Object_Character()
{
	position = pos;
}

Object_Enemy::~Object_Enemy()
{
}

bool Object_Enemy::Start()
{
	bool ret = true;
	//Divide enemy into each type of enemy
	Load("animations/Enemy_Wwisp.tmx");
	Load("animations/Enemy_Kobold.tmx");
	currentAnimation = idle;
	collider = App->collider->AddCollider({ characterPos.x + 56,characterPos.y + 5,25,46 }, COLLIDER_ENEMY);
	
	
	return ret;
}

bool Object_Enemy::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Enemy::Update(float dt)
{
	bool ret = true;
	Draw();
	return ret;
}

bool Object_Enemy::PostUpdate()
{
	bool ret = true;
	return ret;
}

Animation* Object_Enemy::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "IDLE") == 0) { idle = anim; }
	else if (strcmp(anim->name.GetString(), "RUN") == 0) { running = anim; running->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "HURT") == 0) { hurt = anim; hurt->loop = false; }
	else if (strcmp(anim->name.GetString(), "DEAD") == 0) { dead = anim; dead->loop = false; }

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

			anim->sprites[i].AABB_rect = LoadAABB(AABB_object);
			anim->sprites[i].AABB_offset = { anim->sprites[i].AABB_rect.x - (int)roundf(characterPos.x),anim->sprites[i].AABB_rect.y - (int)roundf(characterPos.y) };
			LOG("AABB Rect X:%i Y: %i W:%i H:%i", anim->sprites[i].AABB_rect.x, anim->sprites[i].AABB_rect.y, anim->sprites[i].AABB_rect.w, anim->sprites[i].AABB_rect.h);

			if (AABB_object.next_sibling("object")) {
				AABB_object = AABB_object.next_sibling("object");
			}
		i++;
	}
	return anim;
}

SDL_Rect Object_Enemy::LoadAABB(pugi::xml_node& AABB_object)
{
	SDL_Rect rect;
	p2Point<int> offset{ 0,0 };

	offset.x = AABB_object.attribute("x").as_int();
	offset.y = AABB_object.attribute("y").as_int();
	rect.w = AABB_object.attribute("width").as_int();
	rect.h = AABB_object.attribute("height").as_int();

	while (offset.x - character_tmx_data.tile_width > 0 || offset.x - character_tmx_data.tile_width > character_tmx_data.tile_width) {
		offset.x -= character_tmx_data.tile_width;
	}

	while (offset.y - character_tmx_data.tile_height > 0 || offset.y - character_tmx_data.tile_height > character_tmx_data.tile_height) {
		offset.y -= character_tmx_data.tile_height;
	}

	rect.x = characterPos.x + offset.x;
	rect.y = characterPos.y + offset.y;
	return rect;
}
