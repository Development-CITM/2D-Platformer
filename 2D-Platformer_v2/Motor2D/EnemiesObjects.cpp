#include "EnemiesObjects.h"

Object_Enemy::Object_Enemy(Object_type type) : Object_Character()
{
}

Object_Enemy::~Object_Enemy()
{
}

bool Object_Enemy::Start()
{
	bool ret = true;
	Load("animations/Enemy_Kobold.tmx");
	currentAnimation = idle;
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
	return ret;
}

bool Object_Enemy::PostUpdate()
{
	bool ret = true;
	return ret;
}
