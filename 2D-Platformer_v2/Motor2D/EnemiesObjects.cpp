#include "EnemiesObjects.h"
#include "p2Log.h"
#include "j1Colliders.h"

Object_Enemy::Object_Enemy(Object_type type, p2Point<int> pos) : Object_Character()
{
	characterPos = pos;
	if (type == Object_type::ENEMY_GROUND)
	{
		Load("animations/Enemy_Kobold.tmx");
		collider = App->collider->AddCollider({ characterPos.x + 56,characterPos.y + 5,25,46 }, COLLIDER_ENEMY);
	}
	else if (type == Object_type::ENEMY_FLYING)
	{
		Load("animations/Enemy_Wwisp.tmx");
		collider = App->collider->AddCollider({ characterPos.x+25 ,characterPos.y + 12,25,25 }, COLLIDER_ENEMY);
	}
	currentAnimation = idle;
}

Object_Enemy::~Object_Enemy()
{
}

bool Object_Enemy::Start()
{
	bool ret = true;
	
	
	
	
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
		i++;
	}
	return anim;
}
