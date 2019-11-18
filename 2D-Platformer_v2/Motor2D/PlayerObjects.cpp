#include "PlayerObjects.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "p2Defs.h"
#include "p2Log.h"

Object_Player::Object_Player(int var): Object_Character()
{
	aha = var;
}

Object_Player::~Object_Player()
{}

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


