#include "EnemiesObjects.h"
#include "p2Log.h"
#include "j1Colliders.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Tilesets.h"
#include "j1Scene.h"

Object_Enemy::Object_Enemy(Object_type type, p2Point<int> pos) : Object_Character()
{
	alive = true;
	position = pos;
	type_object = type;
	state = State::IDLE;
	if (type == Object_type::ENEMY_GROUND)
	{
		if (firstkobold)
		{
			Load("animations/Enemy_Kobold.tmx");
			firstkobold = false;
		}		
	}
	else if (type == Object_type::ENEMY_FLYING)
	{
		if (firstwhisp)
		{
			Load("animations/Enemy_Wwisp.tmx");
			firstwhisp = false;
		}
	}
		currentAnimation = idle;
}

Object_Enemy::~Object_Enemy()
{
}

bool Object_Enemy::Start()
{
	bool ret = true;
	
	InitCheckers(type_object);
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
	if (groundChecker) {
		if (App->collider->CheckColliderCollision(groundChecker,COLLIDER_CEILING_CHECKER)) {
			groundChecker->collided = true;
			onGround = true;
		}
		else {
			groundChecker->collided = false;
			onGround = false;
		}
	}
	if (App->collider->CheckColliderCollision(collider, COLLIDER_PLAYER_HIT)) {
		alive = false;

		collider->to_delete = true;
		if (groundChecker) {
			groundChecker->to_delete = true;
		}

	}

	//LOG("Collider to delete: %i", collider->to_delete);
	LOG("Collider: %d", collider);
	LOG("Collider: %d", groundChecker);
	switch (state)
	{

	case State::IDLE:
		StablishPath();
		ChangeAnimation(idle);
		if (!onGround) {
		state = State::FALL;

		}
		break;
	case State::RUNNING:
		StablishPath();
		ChangeAnimation(running);
		if (!onGround) {
			state = State::FALL;
		}
		break;
	case State::ATTACK:
		StablishPath();
		if (attack && canAttack && type_object==Object_type::ENEMY_GROUND) {
			ChangeAnimation(attack);
			if (attack->finished) {
				ChangeAnimation(idle);
				canAttack = false;
				timer = 0;
			}
		}
		if (timer < attackDelay && !canAttack) {
			timer++;
		}
		else {
			timer = 0;
			canAttack = true;
		}
		break;
	case State::DIE:
		break;
	case State::FALL:
		ChangeAnimation(idle);
		if (!onGround) {
			position.y += 2;
		}
		else {
			state = State::IDLE;
			onGround = true;
		}
	
		break;
	}

	if (alive) {
		switch (type_object)
		{
		case ENEMY_GROUND:
			if (groundChecker != nullptr) {
				groundChecker->rect.x = collider->rect.x + groundChecker->offset.x;
				groundChecker->rect.y = collider->rect.y + groundChecker->offset.y;
			}
			collider->rect.x = position.x + 25;
			collider->rect.y = position.y - 8;
			break;
		case ENEMY_FLYING:
			if (currentAnimation == idle) {
				collider->rect.x = position.x + 5;
				collider->rect.y = position.y;
			}
			else if (currentAnimation == running) {
				collider->rect.x = position.x + 15;
				collider->rect.y = position.y;
			}
			break;

		}
	}
	Draw(dt);
	return ret;
}

bool Object_Enemy::PostUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Enemy::CleanUp()
{
	bool ret = true;
	return ret;
}

void Object_Enemy::StablishPath()
{
	iPoint origin;
	iPoint player_pos;

	origin.x = position.x;
	origin.y = position.y;

	player_pos.x = App->entity->RetreivePlayerCollider()->GetPosition().x;
	player_pos.y = App->entity->RetreivePlayerCollider()->GetPosition().y;

	origin = App->tiles->WorldToMap(origin.x, origin.y);
	player_pos = App->tiles->WorldToMap(player_pos.x, player_pos.y);
	
	if (origin.DistanceNoSqrt(player_pos) < 100) {
		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
		App->pathfinding->CreatePath(origin, player_pos);
		if (path->At(1)) {
			MoveToTarget(App->tiles->MapToWorld(path->At(1)->x, path->At(1)->y));

			if (origin.x < player_pos.x) {
				flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			}
			else {
				flip = SDL_RendererFlip::SDL_FLIP_NONE;
			}
			for (uint i = 0; i < path->Count(); ++i)
			{
				if (path->Count() != 0 && App->collider->collider_debug) {
					iPoint pos = App->tiles->MapToWorld(path->At(i)->x, path->At(i)->y);
					App->render->Blit(App->scene->debug_tex, pos.x, pos.y, NULL, App->render->drawsize);
				}
			}
		}
	}
	else {
		App->pathfinding->ClearPath();
	}
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	if ((origin.x - player_pos.x) * (origin.x - player_pos.x) <= 0) {
		currentAnimation = idle;
		state = State::ATTACK;
	}
	//LOG("Count: %i", path->Count());
}

void Object_Enemy::MoveToTarget(p2Point<int> target)
{
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	if (path->At(1) != nullptr) {
		switch (type_object)
		{
		case PLAYER:
			break;
		case ENEMY_GROUND:

			if (position.x < App->entity->RetreivePlayerCollider()->GetPosition().x) {
				position.x += 1;
				currentAnimation = running;
				state = State::RUNNING;
			}
			if (position.x > App->entity->RetreivePlayerCollider()->GetPosition().x) {
				position.x -= 1;
				currentAnimation = running;
				state = State::RUNNING;
			}
			break;
		case ENEMY_FLYING:
			if (position.x < target.x) {
				position.x += 1;
				currentAnimation = running;
				state = State::RUNNING;
			}
			if (position.x > target.x) {
				position.x -= 1;
				currentAnimation = running;
				state = State::RUNNING;
			}

			if (position.y < target.y) {
				position.y += 1;
				currentAnimation = running;
				state = State::RUNNING;
			}
			if (position.y > target.y) {
				position.y -= 1;
				currentAnimation = running;
				state = State::RUNNING;
			}
			break;
		}
	}


}

Animation* Object_Enemy::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "IDLE") == 0) { idle = anim; }
	else if (strcmp(anim->name.GetString(), "RUN") == 0) { running = anim; running->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "HURT") == 0) { hurt = anim; hurt->loop = false; }
	else if (strcmp(anim->name.GetString(), "DEAD") == 0) { dead = anim; dead->loop = false; }
	else if (strcmp(anim->name.GetString(), "ATTACK") == 0) { attack = anim; attack->loop = false; }

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

void Object_Enemy::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}

bool Object_Enemy::InitCheckers(Object_type type)
{
	bool ret = true;
	if (type == Object_type::ENEMY_FLYING)
	{
		collider = App->collider->AddCollider({ position.x,position.y,25,25 }, COLLIDER_ENEMY);
	}
	else if (type == Object_type::ENEMY_GROUND)
	{
		collider = App->collider->AddCollider({ position.x,position.y,25,46 }, COLLIDER_ENEMY);
		groundChecker = App->collider->AddCollider({ position.x,position.y,collider->rect.w - 1 ,4 }, COLLIDER_CEILING_CHECKER, { 1,collider->rect.h });
		groundChecker->checkerType = ColliderChecker::Ground;
	}
	return ret;
}
