#include "PlayerObjects.h"
#include "j1EntityManager.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "j1Colliders.h"
#include "j1Debug.h"
#include "j1App.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Tilesets.h"
#include "j1Audio.h"
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

struct Collider;
Object_Player::Object_Player(pugi::xml_node& object): Object_Character()
{
	absolutePos = { 0,0 };
	SetPos(object);
}

Object_Player::~Object_Player()
{}

bool Object_Player::Start()
{
	bool ret = true;
	Load("animations/Player.tmx");
	App->audio->LoadFx("audio/fx/jump_fx.wav");
	App->audio->LoadFx("audio/fx/double_jump_fx.wav");
	App->audio->LoadFx("audio/fx/water_sfx.wav");
	App->audio->LoadFx("audio/fx/dead_fx.wav");
	
	InitCheckers();
	currentAnimation = disarmed_idle;
	alive = true;
	dead_once = true;
	state = ST_Idle_v2;
	return ret;
}

bool Object_Player::PreUpdate()
{
	bool ret = true;
	velocity_X = position.x;
	velocity_Y = position.y;

	//Hold Movements
	if (canMove) {
		switch (state)
		{
		case ST_Idle:
			HorizontalInputs();
			JumpInput();
			AttackInputs();
			break;
		case ST_Run:
			HorizontalInputs();
			JumpInput();
			AttackInputs();
			break;
		case ST_Jump:
			HorizontalInputs();
			JumpInput();
			break;
		case ST_DoubleJump:
			HorizontalInputs();
			break;
		case ST_Fall:
			HorizontalInputs();
			JumpInput();
			break;
		case ST_MP:
			HorizontalInputs();
			break;
		case ST_LK:
			HorizontalInputs();
			break;
		case ST_Die:
			HorizontalInputs();
			break;
		}
	}
	return true;
	return ret;
}

bool Object_Player::Update(float dt)
{
	bool ret = true;
	velocity_X -= position.x;
	velocity_Y -= position.y;

	if (App->debug->godmode) {
		absolutePos.x = collider->rect.x - App->tiles->culling_Collider->rect.x;
		absolutePos.y = collider->rect.y - App->tiles->culling_Collider->rect.y;
		GodMove();
		UpdateCheckersPosition();
		UpdatePlayerPosition();
		Draw(dt);
		return true;
	}

	UpdateCheckersBools();

	UpdateColliderSize();

	Gravity(dt);

	ChangeStates();

	LogicStateMachine(dt);

	UpdateCheckersPosition();

	UpdatePlayerPosition();

	absolutePos.x = collider->rect.x - App->tiles->culling_Collider->rect.x;
	absolutePos.y = collider->rect.y - App->tiles->culling_Collider->rect.y;

	//LOG("Camera: (%i,%i)", App->render->camera.x, App->render->camera.y);
	LOG("POS: (%i,%i)", position.x, position.y);

	Draw(dt); //Draw all the player
	dt_variable = dt;
	return ret;
}

bool Object_Player::CleanUp()
{
	bool ret = true;

	p2List_item<Mix_Chunk*>* item =App->audio->fx.start;
	{
		for (int i = App->audio->fx.count() - 1; i >= 0; i--)
		{
			Mix_FreeChunk(item->data);
			App->audio->fx.del(App->audio->fx.At(i));
		}
			
	}

	return ret;
}

void Object_Player::LogicStateMachine(float dt)
{
	switch (state)
	{
	case ST_Idle:
		JumpStart(dt);
		break;
	case ST_Walk:
		break;
	case ST_Run:
		HorizontalMove(dt);
		JumpStart(dt);
		break;
	case ST_Jump:
		ChangeAnimation(disarmed_jump);
		canDoubleJump = true;
		if (verticalSpeed_v2 >= 1.f * dt * 45) {
			jumping = false;
			falling = true;
		}
		DoubleJumpStart(dt);
		if (state == CharacterState::ST_Jump) {
			int posY = 0;
			collider->rect.y += (int)roundf(verticalSpeed_v2);
			ceilingChecker->rect.x = collider->rect.x + ceilingChecker->offset.x;
			ceilingChecker->rect.y = collider->rect.y + ceilingChecker->offset.y;
			if (App->collider->CheckColliderCollision(ceilingChecker, Directions::DIR_UP, &posY)) {
				collider->rect.y = posY;
				verticalSpeed_v2 = 0.0f;
			}
		}
		HorizontalMove(dt);
		break;
	case ST_DoubleJump:
		ChangeAnimation(disarmed_double_jump);
		if (verticalSpeed_v2 >= 1.f * dt * 45) {
			jumping = false;
			falling = true;
		}
		if (state == CharacterState::ST_DoubleJump) {
			int posY = 0;
			collider->rect.y += (int)roundf(verticalSpeed_v2);
			ceilingChecker->rect.x = collider->rect.x + ceilingChecker->offset.x;
			ceilingChecker->rect.y = collider->rect.y + ceilingChecker->offset.y;
			if (App->collider->CheckColliderCollision(ceilingChecker, Directions::DIR_UP, &posY)) {
				collider->rect.y = posY;
				verticalSpeed_v2 = 0.0f;
			}
		}
		HorizontalMove(dt);
		break;
	case ST_MP:
		ChangeAnimation(disarmed_mp);
		MP_attackPressed = false;
		if (App->maxcapFrames)
		{
			if (disarmed_mp->current_sprite == 3) {
				collider_attack->Enabled = true;
			}
			else {
				collider_attack->Enabled = false;
			}
		}
		else if (!App->maxcapFrames)
		{
			if (disarmed_mp->current_sprite == 2) {
				collider_attack->Enabled = true;
			}
			else {
				collider_attack->Enabled = false;
			}
		}
		break;
	case ST_LK:
		ChangeAnimation(disarmed_lk);
		LK_attackPressed = false;

		if (App->maxcapFrames)
		{
			if (disarmed_lk->current_sprite == 3) {
				collider_attack->Enabled = true;
			}
			else {
				collider_attack->Enabled = false;
			}
		}
		else if (!App->maxcapFrames)
		{
			if (disarmed_lk->current_sprite == 2) {
				collider_attack->Enabled = true;
			}
			else {
				collider_attack->Enabled = false;
			}
		}
		break;
	case ST_Fall:
		canDoubleJump = true;
		ChangeAnimation(disarmed_fall);
		HorizontalMove(dt);
		DoubleJumpStart(dt);
		break;
	case ST_GrabLedge:
		break;
	case ST_Die:
		ChangeAnimation(disarmed_dead);
		App->debug->CallFade();
		break;
	default:
		break;
	}
}

void Object_Player::JumpStart(float dt)
{
	if (jumpPressed) {
		verticalSpeed_v2 = jumpSpeed_v2 * dt * 37;
		state = CharacterState_v2::ST_Jump_v2;
		jumpPressed = false;
		jumping = true;
		falling = false;
		canDoubleJump = true;
	}
}

void Object_Player::DoubleJumpStart(float dt)
{
	if (doubleJumped) {
		verticalSpeed_v2 = double_jumpSpeed_v2 * dt * 35;
		state = CharacterState_v2::ST_DoubleJump_v2;
		canDoubleJump = false;
		doubleJumped = false;
		jumping = true;
		falling = false;
		doublejumpCount++;
	}
}

void Object_Player::Gravity(float dt)
{
	if (verticalSpeed_v2 > -1.f && verticalSpeed_v2 < 2.f) {
		gravitySpeed_v2 = 0.2f * dt * 60;
	}

	else if (verticalSpeed_v2 > -3.f && verticalSpeed_v2 < -1.f) {
		gravitySpeed_v2 = 0.3f * dt * 40;
	}
	else {
		gravitySpeed_v2 = max_gravitySpeed_v2;
	}

	if (verticalSpeed_v2 < max_verticalSpeed_v2) {
		verticalSpeed_v2 += gravitySpeed_v2 * dt * 40;
	}
	else if (verticalSpeed_v2 > max_verticalSpeed_v2) {
		verticalSpeed_v2 = max_verticalSpeed_v2 * dt * 40;
	}

	//Move and check if we are on Ground
	int posY = 0;
	collider->rect.y += (int)roundf(verticalSpeed_v2);
	groundChecker->rect.y += (int)roundf(verticalSpeed_v2);
	if (App->collider->CheckColliderCollision(groundChecker, Directions::DIR_DOWN, &posY)) {
		collider->rect.y = posY;
		onGround = true;
		canDoubleJump = false;
		doublejumpCount = 0;
	}
}

void Object_Player::ChangeStates()
{
	switch (state)
	{
	case ST_Idle:
		ChangeAnimation(disarmed_idle);
		if (!onGround) {
			state = CharacterState_v2::ST_Fall_v2;
			break;
		}
		if (moveLeft != moveRight && onGround) {
			state = CharacterState_v2::ST_Run_v2;
			break;
		}
		if (jumping) {
			state = CharacterState_v2::ST_Jump_v2;
			break;
		}
		if (MP_attackPressed) {
			state = CharacterState_v2::ST_MP_v2;
			break;
		}
		if (LK_attackPressed) {
			state = CharacterState_v2::ST_LK_v2;
			break;
		}
		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
		}
		
		break;
	case ST_Walk:
		break;
	case ST_Run:
		ChangeAnimation(disarmed_run);
		if (moveLeft == moveRight && onGround) {
			state = CharacterState_v2::ST_Idle_v2;
			break;
		}
		if (!onGround) {
			state = CharacterState_v2::ST_Fall_v2;
			verticalSpeed_v2 = 0.f;
		}
		if (jumping) {
			state = CharacterState_v2::ST_Jump_v2;
			break;
		}
		if (MP_attackPressed) {
			state = CharacterState_v2::ST_MP_v2;
			break;
		}

		if (LK_attackPressed) {
			state = CharacterState_v2::ST_LK_v2;
			break;
		}

		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
			}
		break;

	case ST_Jump:
		if (atCeiling) {
			state = CharacterState_v2::ST_Fall_v2;
			break;
		}
		if (falling) {
			state = CharacterState_v2::ST_Fall_v2;
			break;
		}
		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
		}
		break;
	case ST_DoubleJump:

		if (atCeiling) {
			state = CharacterState_v2::ST_Fall_v2;
			break;
		}
		if (falling) {
			state = CharacterState_v2::ST_Fall_v2;
			break;
		}
		break;
	case ST_MP:
		if (disarmed_mp->finished && moveLeft == moveRight) {
			state = CharacterState_v2::ST_Idle_v2;
		}
		else if (disarmed_mp->finished) {
			state = CharacterState_v2::ST_Run_v2;
		}
		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
		}
		break;
	case ST_LK:
		if (disarmed_lk->finished && moveLeft == moveRight) {
			state = CharacterState_v2::ST_Idle_v2;
		}
		else if (disarmed_lk->finished) {
			state = CharacterState_v2::ST_Run_v2;
		}
		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
		}
		break;
	case ST_Fall:
		if (onGround) {
			state = CharacterState_v2::ST_Idle_v2;
			break;
		}
		if (onGround && moveLeft != moveRight) {
			state = CharacterState_v2::ST_Run_v2;
			break;
		}
		if (!alive) {
			state = CharacterState_v2::ST_Die_v2;
			break;
		}
		break;
	case ST_GrabLedge:
		break;
	case ST_Die:
		break;
	}
}



bool Object_Player::PostUpdate()
{
	bool ret = true;
	float speed = runSpeed_v2 * 2;

	if (runSpeed_v2 < 0) 
		speed *= -1.f;
	
	if (absolutePos.x < 160 && -App->render->camera.x > App->scene->camera_limit_left) {
		App->render->MoveCamera({ (int)roundf(speed * ceil(dt_variable * 50)), 0 });
	}
	if (absolutePos.x > 285 && -App->render->camera.x < App->scene->camera_limit_right) {
		App->render->MoveCamera({ -(int)roundf(speed * ceil(dt_variable * 50)), 0 });
	}
	if (absolutePos.y < 100) {
		App->render->MoveCamera({ 0, (int)roundf(6 * ceil(dt_variable * 32)) });
	}
	if (absolutePos.y > 260 && -App->render->camera.y < App->scene->camera_limit_bot) {
		App->render->MoveCamera({ 0, -(int)roundf(10 * ceil(dt_variable * 32)) });
	}
	return ret;
}

bool Object_Player::InitCheckers()
{
	bool ret = true;
	// CHANGE CALLBACK TYPE IN ORDER TO CREATE CHECKERS ONCE ALL IS DONE AND WE CAN IGNORE PLAYERCPP
	
	collider = App->collider->AddCollider({ position.x,position.y,20,45 }, COLLIDER_PLAYER, { 0,0 });

	groundChecker = App->collider->AddCollider({ position.x,position.y,collider->rect.w - 1,5 }, COLLIDER_CEILING_CHECKER, { 1,collider->rect.h });
	groundChecker->checkerType = ColliderChecker::Ground;

	ceilingChecker = App->collider->AddCollider({ position.x,position.y,collider->rect.w - 1 ,4 }, COLLIDER_CEILING_CHECKER, { 1,-4 });
	ceilingChecker->checkerType = ColliderChecker::Top;

	rightChecker = App->collider->AddCollider({ position.x,position.y,4,collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { collider->rect.w,5 });
	rightChecker->checkerType = ColliderChecker::Right;

	leftChecker = App->collider->AddCollider({ position.x,position.y,4,collider->rect.h - 10 }, COLLIDER_CEILING_CHECKER, { -3,5 });
	leftChecker->checkerType = ColliderChecker::Left;

	collider_attack = App->collider->AddCollider({ position.x,position.y,20,8 }, COLLIDER_PLAYER_HIT, { 27,16 });
	collider_attack->Enabled = false;
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
	else if (strcmp(anim->name.GetString(), "DISARMED_DEAD") == 0) { disarmed_dead = anim; disarmed_dead->loop = false; }

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
			anim->sprites[i].AABB_offset = { anim->sprites[i].AABB_rect.x - (int)roundf(position.x),anim->sprites[i].AABB_rect.y - (int)roundf(position.y) };
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

	rect.x = position.x + offset.x;
	rect.y = position.y + offset.y;
	return rect;
}


void Object_Player::SetPos(pugi::xml_node& object)
{
	if (App->scene->loading == false)
	{
		if (App->scene->swapping)
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
			{
				SetPlayerPosFromCurrentLevel(it);
			}
		}
		else
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
			{
				if (strcmp(it.attribute("name").as_string(), "player_pos_x") == 0)
				{
					position.x = it.attribute("value").as_int();
					if (collider != nullptr)
					{
						collider->rect.x = it.attribute("value").as_int();
					}

				}
				if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
				{
					position.y = it.attribute("value").as_int();
					if (collider != nullptr)
					{
						collider->rect.y = it.attribute("value").as_int();
					}
				}
			}
		}
	}
	else
	{
		position.x = App->scene->player_backup_x;
		position.y = App->scene->player_backup_y;
		if (collider != nullptr)
		{
			collider->rect.x = App->scene->player_backup_collider_x;

			collider->rect.y = App->scene->player_backup_collider_y;
		}
	}
}

void Object_Player::SetPlayerPosFromCurrentLevel(pugi::xml_node& it)
{
	char* pos_x = "player_pos_x";
	char* pos_y = "player_pos_y";

	if (App->scene->current_level == App->scene->A1)
	{
		pos_x = "player_pos_x_swap_from_A1";
		pos_y = "player_pos_y_swap_from_A1";

	}
	else if (App->scene->current_level == App->scene->A2)
	{
		pos_x = "player_pos_x_swap_from_A2";
		pos_y = "player_pos_y_swap_from_A2";
	}
	else if (App->scene->current_level == App->scene->A3)
	{
		pos_x = "player_pos_x_swap_from_A3";
		pos_y = "player_pos_y_swap_from_A3";
	}
	else if (App->scene->current_level == App->scene->A5)
	{
		pos_x = "player_pos_x_swap_from_A5";
		pos_y = "player_pos_y_swap_from_A5";
	}
	else if (App->scene->current_level == App->scene->A6)
	{
		pos_x = "player_pos_x_swap_from_A6";
		pos_y = "player_pos_y_swap_from_A6";
	}

	if (strcmp(it.attribute("name").as_string(), pos_x) == 0)
	{
		position.x = it.attribute("value").as_int();
		if (collider != nullptr)
		{
			collider->rect.x = it.attribute("value").as_int();
		}
	}
	if (strcmp(it.attribute("name").as_string(), pos_y) == 0)
	{
		position.y = it.attribute("value").as_int();
		if (collider != nullptr)
		{
			collider->rect.y = it.attribute("value").as_int();
		}
	}
}

void Object_Player::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}

void Object_Player::HorizontalMove(float dt)
{
	if (moveLeft == moveRight) {
		runSpeed_v2 = 0.f;
	}
	else if (moveRight) {
		int posX = 0;
		collider->rect.x += (int)roundf(runSpeed_v2 * ceil(dt * 50));
		if (App->collider->CheckColliderCollision(rightChecker, Directions::DIR_RIGHT, &posX)) {
			if (posX != 0) {
				runSpeed_v2 = 0.f;
				collider->rect.x = posX;
			}
		}
		else {
			runSpeed_v2 = max_runSpeed_v2;
		}
	}
	else if (moveLeft) {
		int posX = 0;
		collider->rect.x += (int)roundf(runSpeed_v2 * ceil(dt * 50));
		if (App->collider->CheckColliderCollision(leftChecker, Directions::DIR_LEFT, &posX)) {
			if (posX != 0) {
				runSpeed_v2 = 0.f;
				collider->rect.x = posX;
			}

		}
		else {
			runSpeed_v2 = -max_runSpeed_v2;
		}
	}
	if (runSpeed_v2 < 0.f) {
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}
	else if (runSpeed_v2 > 0.f) {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
	}
}

void Object_Player::UpdatePlayerPosition()
{
	//Update Player Pos
	if (App->scene->loading == false)
	{
		position.x = collider->rect.x - (int)roundf(character_tmx_data.tile_width * 0.5) - 2;
		position.y = collider->rect.y - colliderOffsetY1;
	}
}

void Object_Player::UpdateCheckersPosition()
{
	//Update Checkers position

	ceilingChecker->rect.x = collider->rect.x + ceilingChecker->offset.x;
	ceilingChecker->rect.y = collider->rect.y + ceilingChecker->offset.y;

	groundChecker->rect.x = collider->rect.x + groundChecker->offset.x;
	groundChecker->rect.y = collider->rect.y + groundChecker->offset.y;

	leftChecker->rect.x = collider->rect.x + leftChecker->offset.x;
	leftChecker->rect.y = collider->rect.y + leftChecker->offset.y;

	rightChecker->rect.x = collider->rect.x + rightChecker->offset.x;
	rightChecker->rect.y = collider->rect.y + rightChecker->offset.y;

	if (flip == SDL_RendererFlip::SDL_FLIP_NONE) {
		collider_attack->rect.x = collider->rect.x + collider_attack->offset.x;

	}
	else if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		collider_attack->rect.x = collider->rect.x - collider_attack->offset.x + 6;
	}
	if (currentAnimation == disarmed_lk) {
		collider_attack->rect.y = collider->rect.y + collider_attack->offset.y - 15;
		collider_attack->rect.w = 12;
		collider_attack->rect.h = 25;
	}
	else {
		collider_attack->rect.y = collider->rect.y + collider_attack->offset.y;
		collider_attack->rect.w = 20;
		collider_attack->rect.h = 8;
	}
}

void Object_Player::UpdateCheckersBools()
{
	//Check Collisions
	if (App->collider->CheckColliderCollision(rightChecker)) {
		rightChecker->collided = true;
	}
	else {
		rightChecker->collided = false;
	}
	if (App->collider->CheckColliderCollision(leftChecker)) {
		leftChecker->collided = true;
	}
	else {
		leftChecker->collided = false;
	}
	if (App->collider->CheckColliderCollision(groundChecker)) {
		groundChecker->collided = true;
	}
	else {
		groundChecker->collided = false;
		onGround = false;
	}

	if (App->collider->CheckColliderCollision(ceilingChecker)) {
		ceilingChecker->collided = true;
	}
	else {
		ceilingChecker->collided = false;
	}
	if (App->collider->CheckColliderCollision(collider, COLLIDER_DEAD)) {
		if (dead_once)
		{
			App->audio->PlayFx(4);
			dead_once = false;
		}
		alive = false;
	}
	if (App->collider->CheckColliderCollision(collider, COLLIDER_ENEMY)) {
		if (dead_once)
		{
			App->audio->PlayFx(4);
			dead_once = false;
		}
		alive = false;
	}
		
}

void Object_Player::UpdateColliderSize()
{
	if (state == ST_Fall) {
		collider->rect.h = 40;
		groundChecker->offset.y = collider->rect.h;
		leftChecker->rect.h = rightChecker->rect.h = collider->rect.h - 10;
	}
	else {
		collider->rect.h = 45;
		groundChecker->offset.y = collider->rect.h;
		leftChecker->rect.h = rightChecker->rect.h = collider->rect.h - 10;
	}
}

void Object_Player::JumpInput()
{
	if (App->debug->input)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && onGround && !atCeiling) {
			App->audio->PlayFx(1);
			jumpPressed = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDoubleJump && doublejumpCount == 0 && !atCeiling) {
			App->audio->PlayFx(2);
			doubleJumped = true;
		}
	}
}

void Object_Player::HorizontalInputs()
{
	if (App->debug->input)
	{
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			moveRight = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
			moveRight = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			moveLeft = true;
		}
		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
			moveLeft = false;
			//direction = DIR_UP;
		}
	}
}

void Object_Player::AttackInputs()
{
	if (App->debug->input)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			MP_attackPressed = true;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
			LK_attackPressed = true;
		}
	}
}

void Object_Player::GodMove()
{

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		collider->rect.x += 4;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		collider->rect.x -= 4;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		collider->rect.y -= max_verticalSpeed_v2;
	}	
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		collider->rect.y += max_verticalSpeed_v2;
	}
}


