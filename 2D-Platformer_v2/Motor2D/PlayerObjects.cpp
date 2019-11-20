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
	state = ST_Idle_v2;

	currentAnimation = disarmed_idle;
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
	return true;
	/*bool ret = true;
	player_Collider = App->collider->AddCollider({ characterPos.x,characterPos.y,20,45 }, COLLIDER_PLAYER, { 0,0 }, this);

	groundChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,player_Collider->rect.w - 1,5 }, COLLIDER_CEILING_CHECKER, { 1,player_Collider->rect.h }, this);
	groundChecker->checkerType = ColliderChecker::Ground;

	ceilingChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,player_Collider->rect.w - 1 ,4 }, COLLIDER_CEILING_CHECKER, { 1,-4 }, this);
	ceilingChecker->checkerType = ColliderChecker::Top;

	rightChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,4,player_Collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { player_Collider->rect.w,5 }, this);
	rightChecker->checkerType = ColliderChecker::Right;

	leftChecker = App->collider->AddCollider({ characterPos.x,characterPos.y,4,player_Collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { -3,5 }, this);
	leftChecker->checkerType = ColliderChecker::Left;
	return ret;*/
}

Animation* Object_Player::LoadAnimation(pugi::xml_node& obj_group)
{
	
	return nullptr;
}


