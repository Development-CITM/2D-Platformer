#include "PlayerObjects.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "j1Colliders.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"

Object_Player::Object_Player(int var): Object_Character()
{
	aha = var;
}

Object_Player::~Object_Player()
{}

bool Object_Player::Start()
{
	bool ret = true;
	Load("animations/Player.tmx");
	//InitCheckers();
	currentAnimation = disarmed_idle;
	state = ST_Idle_v2;
	return ret;
}

bool Object_Player::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Player::Update(float dt)
{
	Object_Dynamic::Draw();
	bool ret = true;
	aha++;
	return ret;
}

bool Object_Player::PostUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Player::InitCheckers()
{
	bool ret = true;
	// CHANGE CALLBACK TYPE IN ORDER TO CREATE CHECKERS ONCE ALL IS DONE AND WE CAN IGNORE PLAYERCPP
	//
	//player_Collider = App->collider->AddCollider({ characterPos.x,characterPos.y,20,45 }, COLLIDER_PLAYER, { 0,0 }, this);

	//groundChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,player_Collider->rect.w - 1,5 }, COLLIDER_CEILING_CHECKER, { 1,player_Collider->rect.h }, this);
	//groundChecker->checkerType = ColliderChecker::Ground;

	//ceilingChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,player_Collider->rect.w - 1 ,4 }, COLLIDER_CEILING_CHECKER, { 1,-4 }, this);
	//ceilingChecker->checkerType = ColliderChecker::Top;

	//rightChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,4,player_Collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { player_Collider->rect.w,5 }, this);
	//rightChecker->checkerType = ColliderChecker::Right;

	//leftChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,4,player_Collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { -3,5 }, this);
	//leftChecker->checkerType = ColliderChecker::Left;
	return ret;
}

Animation* Object_Player::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "DISARMED_IDLE") == 0) { disarmed_idle = anim; }
	else if (strcmp(anim->name.GetString(), "DISARMED_RUN") == 0) { disarmed_run = anim; disarmed_run->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "DISARMED_JUMP") == 0) { disarmed_jump = anim; disarmed_jump->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_DJUMP") == 0) { disarmed_double_jump = anim; disarmed_double_jump->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_FALL") == 0) { disarmed_fall = anim; }
	else if (strcmp(anim->name.GetString(), "DISARMED_MP") == 0) { disarmed_mp = anim; disarmed_mp->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_LK") == 0) { disarmed_lk = anim; disarmed_lk->loop = false; }

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

SDL_Rect Object_Player::LoadAABB(pugi::xml_node& AABB_object)
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


