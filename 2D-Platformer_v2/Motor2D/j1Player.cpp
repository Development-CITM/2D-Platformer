#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Colliders.h"
#include "j1Tilesets.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1Scene.h"
#include <math.h>

#pragma region Constructor/Awake/Start
struct Collider;

j1Player::j1Player()
{
	//Set module name
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& conf)
{
	bool ret = true; 
	
	LOG("Loading Player...");
	pugi::xml_node player_values = conf;
	return ret;	
}

bool j1Player::Start()
{
	//Load Player tmx (it contains animations and colliders properties)
	Load("animations/Player.tmx");
	state = ST_Idle;

	currentAnimation = disarmed_idle;

	return true;
}

#pragma endregion

#pragma region PreUpdate/Update/PostUpdate

bool j1Player::PreUpdate()
{
	velocity_X = playerPos.x;
	velocity_Y = playerPos.y;

	//Hold Movements
	if (canMove) {
		switch (state)
		{
		case ST_Idle:
			HorizontalInputs();
			JumpInput();
			break;
		case ST_Run:
			HorizontalInputs();
			JumpInput();
			break;
		case ST_Jump:
			HorizontalInputs();
			break;
		case ST_Fall:
			HorizontalInputs();
			break;
		}
	}
	return true;
}

bool j1Player::Update(float dt)
{
	LOG("Playerpos X: %i",playerPos.x);
	LOG("Playerpos Y: %i",playerPos.y);
	velocity_X -= playerPos.x;
	velocity_Y -= playerPos.y;


	ChangeStatesAndAnimations();

	numCurrentAnimation = currentAnimation->GetSprite();
	
	UpdateCheckersBools();

	JumpMove();

	HorizontalMove();

	UpdatePlayerPosition();

	UpdateCheckersPosition();

	Draw(); //Draw all the player

	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

#pragma endregion

#pragma region CleanUp

bool j1Player::CleanUp()
{
	//// Remove all layers
	p2List_item<ObjectLayer*>* item;
	item = player_tmx_data.object_Layers.start;

	while (item != NULL)
	{
		for (int i = 0; i < item->data->rects.count(); i++)
		{
			delete item->data->rects[i];
		}
		RELEASE(item->data);
		item = item->next;
	}
	player_tmx_data.object_Layers.clear();

	return true;
}

#pragma endregion

#pragma region Loads

bool j1Player::Load(const char* file_name)
{
	bool ret = true;
	pugi::xml_document	player_file;
	pugi::xml_parse_result result = player_file.load_file(file_name);

	player_Collider = App->collider->AddCollider({ playerPos.x,playerPos.y,20,45 }, COLLIDER_PLAYER, { 0,0 }, this);

	groundChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,player_Collider->rect.w - 1,5 }, COLLIDER_CEILING_CHECKER, { 1,player_Collider->rect.h }, this);
	groundChecker->checkerType = ColliderChecker::Ground;

	ceilingChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,player_Collider->rect.w-1 ,4 }, COLLIDER_CEILING_CHECKER, { 1,-4 }, this);
	ceilingChecker->checkerType = ColliderChecker::Top;

	rightChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,4,player_Collider->rect.h-10}, COLLIDER_CEILING_CHECKER, { player_Collider->rect.w,5 }, this);
	rightChecker->checkerType = ColliderChecker::Right;

	leftChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,4,player_Collider->rect.h -10}, COLLIDER_CEILING_CHECKER, { -3,5 }, this);
	leftChecker->checkerType = ColliderChecker::Left;



	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	//Load info
	if (ret == true) {

		pugi::xml_node	player_node = player_file.child("map");

		LoadPlayerTMX(player_node);

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

bool j1Player::LoadPlayerTMX(pugi::xml_node& player_node)
{
	bool ret = true;

	if (player_node == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		player_tmx_data.width = player_node.attribute("width").as_int();
		player_tmx_data.height = player_node.attribute("height").as_int();
		player_tmx_data.tile_width = player_node.attribute("tilewidth").as_int();
		player_tmx_data.tile_height = player_node.attribute("tileheight").as_int();

		p2SString image_source("animations/");
		image_source += player_node.child("imagelayer").child("image").attribute("source").as_string();
		player_tmx_data.texture = App->tex->Load(image_source.GetString());
		if (player_tmx_data.texture) {
			LOG("Player Texture succesfull loaded");
		}
		else {
			LOG("Error loading player texture");
			ret = false;
		}
	}
	return ret;
}

Animation* j1Player::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "DISARMED_IDLE") == 0) { disarmed_idle = anim; }
	else if (strcmp(anim->name.GetString(), "DISARMED_RUN") == 0) { disarmed_run = anim; disarmed_run->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "DISARMED_JUMP") == 0) { disarmed_jump = anim; disarmed_jump->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_FALL") == 0) { disarmed_fall = anim; }

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
		anim->sprites[i].AABB_offset = { anim->sprites[i].AABB_rect.x - (int)roundf(playerPos.x),anim->sprites[i].AABB_rect.y - (int)roundf( playerPos.y) };
		LOG("AABB Rect X:%i Y: %i W:%i H:%i", anim->sprites[i].AABB_rect.x, anim->sprites[i].AABB_rect.y, anim->sprites[i].AABB_rect.w, anim->sprites[i].AABB_rect.h);

		if (AABB_object.next_sibling("object")) {
			AABB_object = AABB_object.next_sibling("object");
		}
		i++;
	}


	return anim;
}

SDL_Rect j1Player::LoadAABB(pugi::xml_node& AABB_object)
{
	SDL_Rect rect;
	p2Point<int> offset{ 0,0 };

	offset.x = AABB_object.attribute("x").as_int();
	offset.y = AABB_object.attribute("y").as_int();
	rect.w = AABB_object.attribute("width").as_int();
	rect.h = AABB_object.attribute("height").as_int();

	while (offset.x - player_tmx_data.tile_width > 0 || offset.x - player_tmx_data.tile_width > player_tmx_data.tile_width) {
		offset.x -= player_tmx_data.tile_width;
	}

	while (offset.y - player_tmx_data.tile_height > 0 || offset.y - player_tmx_data.tile_height > player_tmx_data.tile_height) {
		offset.y -= player_tmx_data.tile_height;
	}

	rect.x = playerPos.x + offset.x;
	rect.y = playerPos.y + offset.y;

	return rect;

}

void j1Player::SetPlayerPos(pugi::xml_node& object)
{
	if (App->scene->loading == false)
	{
		for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) 
		{
			if (strcmp(it.attribute("name").as_string(), "player_pos_x") == 0)
			{
				playerPos.x = it.attribute("value").as_int();
				if (player_Collider != nullptr)
				{
					player_Collider->rect.x = it.attribute("value").as_int();
				}
				
			}
			if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
			{
				playerPos.y = it.attribute("value").as_int();
				if (player_Collider != nullptr)
				{
					player_Collider->rect.y = it.attribute("value").as_int();
				}
			}
		}
	}
	else
	{
		player_Collider->rect.x = playerPos.x;
		player_Collider->rect.y = playerPos.y;
	}

}

#pragma endregion

#pragma region OnCollision

void j1Player::OnCollision(Collider* c1, Collider* c2)
{
	
}

#pragma endregion

#pragma region Inputs

void j1Player::JumpInput()
{

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && onGround && !atCeiling) {
		jumpPressed = true;
	}
}

void j1Player::HorizontalInputs()
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

#pragma endregion

#pragma region UpdateColliders

void j1Player::UpdateCheckersPosition()
{
	//Update Checkers position

	ceilingChecker->rect.x = player_Collider->rect.x + ceilingChecker->offset.x;
	ceilingChecker->rect.y = player_Collider->rect.y + ceilingChecker->offset.y;

	groundChecker->rect.x = player_Collider->rect.x + groundChecker->offset.x;
	groundChecker->rect.y = player_Collider->rect.y + groundChecker->offset.y;

	leftChecker->rect.x = player_Collider->rect.x + leftChecker->offset.x;
	leftChecker->rect.y = player_Collider->rect.y + leftChecker->offset.y;

	rightChecker->rect.x = player_Collider->rect.x + rightChecker->offset.x;
	rightChecker->rect.y = player_Collider->rect.y + rightChecker->offset.y;
}

void j1Player::UpdateCheckersBools()
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
}

#pragma endregion

#pragma region Movement

void j1Player::JumpMove()
{
	if (jumpPressed) {
		verticalSpeed = jumpSpeed;
		state = CharacterState::ST_Jump;
		jumpPressed = false;
		onGround = false;
		jumping = true;
	}
	if (verticalSpeed >= 1.f) {
		jumping = false;
	}
	//Gravity
	if (verticalSpeed > -1.f && verticalSpeed < 2.f) {
		gravitySpeed = 0.2f;
	}
	else {
		gravitySpeed = max_gravitySpeed;
	}

	if (!jumping) {
		if (verticalSpeed < 6.f) {
			verticalSpeed += gravitySpeed;
		}
		else if (verticalSpeed > 6.f) {
			verticalSpeed = 6.f;
		}

		int posY = 0;

		player_Collider->rect.y += (int)roundf(verticalSpeed);
		groundChecker->rect.y += (int)roundf(verticalSpeed);
		if (App->collider->CheckColliderCollision(groundChecker, Directions::DIR_DOWN, &posY)) {
			player_Collider->rect.y = posY;
			onGround = true;
		}
	}
	else if (!onGround && jumping) {
		int posY = 0;
		if (verticalSpeed < 6.f) {
			verticalSpeed += gravitySpeed;
			player_Collider->rect.y += (int)roundf(verticalSpeed);

			ceilingChecker->rect.x = player_Collider->rect.x + ceilingChecker->offset.x;
			ceilingChecker->rect.y = player_Collider->rect.y + ceilingChecker->offset.y;
			if (App->collider->CheckColliderCollision(ceilingChecker, Directions::DIR_UP, &posY)) {
				player_Collider->rect.y = posY;
				verticalSpeed = 0.0f;
			}
		}
	}
}

void j1Player::HorizontalMove()
{
	if (moveLeft == moveRight) {
		runSpeed = 0.f;
	}
	else if (moveRight) {
		int posX = 0;
		player_Collider->rect.x += (int)roundf(runSpeed);
		if (App->collider->CheckColliderCollision(rightChecker, Directions::DIR_RIGHT, &posX)) {
			if (posX != 0) {
				runSpeed = 0.f;
				player_Collider->rect.x = posX;
			}
		}
		else {
			runSpeed = max_runSpeed;
		}
	}
	else if (moveLeft) {
		int posX = 0;
		player_Collider->rect.x += (int)roundf(runSpeed);
		if (App->collider->CheckColliderCollision(leftChecker, Directions::DIR_LEFT, &posX)) {
			if (posX != 0) {
				runSpeed = 0.f;
				player_Collider->rect.x = posX;
			}

		}
		else {
			runSpeed = -max_runSpeed;
		}
	}
}

void j1Player::UpdatePlayerPosition()
{
	//Update Player Pos
	if (App->scene->loading == false)
	{
		playerPos.x = player_Collider->rect.x - (int)roundf(player_tmx_data.tile_width * 0.5) - 2;
		playerPos.y = player_Collider->rect.y - colliderOffsetY1;
	}
}

#pragma endregion

#pragma region Change States/Animations

void j1Player::ChangeStatesAndAnimations()
{
	//Change animation && state
	if (runSpeed == 0.f && onGround) {
		ChangeAnimation(disarmed_idle);
		state = CharacterState::ST_Idle;
	}
	else if (runSpeed != 0.f && onGround) {
		if (runSpeed < 0.f) {
			flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		}
		else if (runSpeed > 0.f) {
			flip = SDL_RendererFlip::SDL_FLIP_NONE;
		}
		ChangeAnimation(disarmed_run);
		state = CharacterState::ST_Run;
	}
	else if (runSpeed != 0) {
		if (runSpeed < 0.f) {
			flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		}
		else if (runSpeed > 0.f) {
			flip = SDL_RendererFlip::SDL_FLIP_NONE;
		}
	}

	if (jumping) {
		ChangeAnimation(disarmed_jump);
		state = CharacterState::ST_Jump;
	}
	else if (!jumping && !onGround) {
		ChangeAnimation(disarmed_fall);
		state = CharacterState::ST_Fall;
		player_Collider->rect.h = 40;
		groundChecker->offset.y = player_Collider->rect.h;
		leftChecker->rect.h = rightChecker->rect.h = player_Collider->rect.h - 10;
	}
	else {
		player_Collider->rect.h = 45;
		groundChecker->offset.y = player_Collider->rect.h;
		leftChecker->rect.h = rightChecker->rect.h = player_Collider->rect.h - 10;
	}
	if (previous_state != state) {
		previous_state = state;
		LOG("State: %i", state);
	}

}

void j1Player::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}
#pragma endregion

#pragma region Draw

void j1Player::Draw()
{

	if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		disarmed_run->offset.x = 5;
	}
	else {
		disarmed_run->offset.x = -5;
	}

	if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		disarmed_jump->offset.x = 5;
	}
	else {
		disarmed_jump->offset.x = -5;
	}

	if (flip == SDL_RendererFlip::SDL_FLIP_HORIZONTAL) {
		disarmed_fall->offset.x = 5;
	}
	else {
		disarmed_fall->offset.x = -5;
	}
	
	App->render->Blit(player_tmx_data.texture, playerPos.x + currentAnimation->offset.x, playerPos.y, &currentAnimation->sprites[numCurrentAnimation].rect,2.f,true,flip);
}

#pragma endregion