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
#include "Brofiler/Brofiler.h"
#include <math.h>

#pragma region Constructor/Awake/Start
struct Collider;

j1Player::j1Player()
{
	//Set module name
	name.create("player");
	absolutePos = { 0,0 };
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
	App->render->SetCameraPos(playerPos);

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
		}
	}
	return true;
}

bool j1Player::Update(float dt)
{

	velocity_X -= playerPos.x;
	velocity_Y -= playerPos.y;



	
	UpdateCheckersBools();

	UpdateColliderSize();

	Gravity(dt);

	ChangeStates();

	LogicStateMachine(dt);

	UpdateCheckersPosition();

	UpdatePlayerPosition();

	if (previous_state != state) {
		previous_state = state;
		LOG("State: %i");
	}

	absolutePos.x = player_Collider->rect.x - App->tiles->culling_Collider->rect.x;
	absolutePos.y = player_Collider->rect.y - App->tiles->culling_Collider->rect.y;


	//Update Camera Position
	//App->render->MoveCamera({ -2,2 });

	LOG("Camera: (%i,%i)", App->render->camera.x, App->render->camera.y);
	LOG("Culling: (%i,%i)", App->tiles->culling_Collider->rect.x, App->tiles->culling_Collider->rect.y);
	LOG("Player: (%i,%i)", playerPos.x, playerPos.y);
	LOG("Absolute Pos: (%i,%i)", absolutePos.x, absolutePos.y);

	Draw(); //Draw all the player
	dt_variable = dt;
	return true;
}


bool j1Player::PostUpdate()
{
	float speed = runSpeed *2;
	if (runSpeed < 0) {
		speed *= -1.f;
	}

	if (App->player->absolutePos.x < 160) {
		App->render->MoveCamera({ (int)roundf(speed * ceil(dt_variable * 50)), 0 });
	}	

	if (App->player->absolutePos.x > 285) {
		App->render->MoveCamera({ -(int)roundf(speed * ceil(dt_variable * 50)), 0 });
	}

	if (App->player->absolutePos.y < 100) {
		App->render->MoveCamera({ 0, (int)roundf(6* ceil(dt_variable * 50)) });
	}	
	
	if (App->player->absolutePos.y > 260) {
		App->render->MoveCamera({ 0, -(int)roundf(10 * ceil(dt_variable * 50)) });
	}





	return true;
}

#pragma endregion

void j1Player::LogicStateMachine(float dt)
{
	switch (state)
	{
	case ST_Idle:
		JumpStart();
		break;
	case ST_Walk:
		break;
	case ST_Run:
		HorizontalMove(dt);
		JumpStart();
		break;
	case ST_Jump:
		ChangeAnimation(disarmed_jump);
		canDoubleJump = true;
		if (verticalSpeed >= 1.f) {
			jumping = false;
			falling = true;
		}
		DoubleJumpStart();
		if (state == CharacterState::ST_Jump) {
			int posY = 0;
			player_Collider->rect.y += (int)roundf(verticalSpeed);
			ceilingChecker->rect.x = player_Collider->rect.x + ceilingChecker->offset.x;
			ceilingChecker->rect.y = player_Collider->rect.y + ceilingChecker->offset.y;
			if (App->collider->CheckColliderCollision(ceilingChecker, Directions::DIR_UP, &posY)) {
				player_Collider->rect.y = posY;
				verticalSpeed = 0.0f;
			}
		}
		HorizontalMove(dt);
		break;
	case ST_DoubleJump:
		ChangeAnimation(disarmed_double_jump);
		if (verticalSpeed >= 1.f) {
			jumping = false;
			falling = true;
		}
		if (state == CharacterState::ST_DoubleJump) {
			int posY = 0;
			player_Collider->rect.y += (int)roundf(verticalSpeed);
			ceilingChecker->rect.x = player_Collider->rect.x + ceilingChecker->offset.x;
			ceilingChecker->rect.y = player_Collider->rect.y + ceilingChecker->offset.y;
			if (App->collider->CheckColliderCollision(ceilingChecker, Directions::DIR_UP, &posY)) {
				player_Collider->rect.y = posY;
				verticalSpeed = 0.0f;
			}
		}
		HorizontalMove(dt);
		break;
	case ST_MP:
		ChangeAnimation(disarmed_mp);
		MP_attackPressed = false;
		break;
	case ST_LK:
		ChangeAnimation(disarmed_lk);
		LK_attackPressed = false;
		break;
	case ST_Fall:
		canDoubleJump = true;
		ChangeAnimation(disarmed_fall);
		HorizontalMove(dt);
		DoubleJumpStart();
		break;
	case ST_GrabLedge:
		break;
	default:
		break;
	}
}

void j1Player::JumpStart()
{
	if (jumpPressed) {
		verticalSpeed = jumpSpeed;
		state = CharacterState::ST_Jump;
		jumpPressed = false;
		jumping = true;
		falling = false;
		canDoubleJump = true;

	}
}

void j1Player::DoubleJumpStart()
{
	if (doubleJumped) {
		verticalSpeed = double_jumpSpeed;
		state = CharacterState::ST_DoubleJump;
		canDoubleJump = false;
		doubleJumped = false;
		jumping = true;
		falling = false;
		doublejumpCount++;
	}
}

void j1Player::Gravity(float dt)
{

	if (verticalSpeed > -1.f && verticalSpeed < 2.f) {
		gravitySpeed = 0.2f;
	}	
	
	else if (verticalSpeed > -3.f && verticalSpeed < -1.f) {
		gravitySpeed = 0.3f;
	}
	else {
		gravitySpeed = max_gravitySpeed;
	}
	
	if (verticalSpeed < max_verticalSpeed) {
			verticalSpeed += gravitySpeed;
	}
	else if (verticalSpeed > max_verticalSpeed) {
		verticalSpeed = max_verticalSpeed;
	}	

	//Move and check if we are on Ground
	int posY = 0;
	player_Collider->rect.y += (int)roundf(verticalSpeed * dt * 60);
	groundChecker->rect.y += (int)roundf(verticalSpeed * dt * 60);
	if (App->collider->CheckColliderCollision(groundChecker, Directions::DIR_DOWN, &posY)) {
		player_Collider->rect.y = posY;
		onGround = true;
		canDoubleJump = false;
		doublejumpCount = 0;
	}
}

void j1Player::ChangeStates()
{
	switch (state)
	{
	case ST_Idle:
		ChangeAnimation(disarmed_idle);
		if (!onGround) {
			state = CharacterState::ST_Fall;
			break;
		}
		if (moveLeft != moveRight && onGround) {
			state = CharacterState::ST_Run;
			break;
		}
		if (jumping) {
			state = CharacterState::ST_Jump;
			break;
		}
		if (MP_attackPressed) {
			state = CharacterState::ST_MP;
			break;
		}	
		if (LK_attackPressed) {
			state = CharacterState::ST_LK;
			break;
		}
		break;
	case ST_Walk:
		break;
	case ST_Run:
		ChangeAnimation(disarmed_run);
		if (moveLeft == moveRight && onGround) {
			state = CharacterState::ST_Idle;
			break;
		}
		if (!onGround) {
			state = CharacterState::ST_Fall;
			verticalSpeed = 0.f;
		}
		if (jumping) {
			state = CharacterState::ST_Jump;
			break;
		}
		if (MP_attackPressed) {
			state = CharacterState::ST_MP;
			break;
		}

		if (LK_attackPressed) {
			state = CharacterState::ST_LK;
			break;
		}
		break;
	case ST_Jump:
		if (atCeiling) {
			state = CharacterState::ST_Fall;
			break;
		}
		if (falling) {
			state = CharacterState::ST_Fall;
			break;
		}
		break;
	case ST_DoubleJump:

		if (atCeiling) {
			state = CharacterState::ST_Fall;
			break;
		}
		if (falling) {
			state = CharacterState::ST_Fall;
			break;
		}
		break;
	case ST_MP:
		if (disarmed_mp->finished && moveLeft == moveRight) {
			state = CharacterState::ST_Idle;
		}
		else if (disarmed_mp->finished) {
			state = CharacterState::ST_Run;
		}
		break;
	case ST_LK:
		if (disarmed_lk->finished && moveLeft == moveRight) {
			state = CharacterState::ST_Idle;
		}
		else if (disarmed_lk->finished) {
			state = CharacterState::ST_Run;
		}
		break;
	case ST_Fall:
		if (onGround) {
			state = CharacterState::ST_Idle;
			break;
		}
		if (onGround && moveLeft != moveRight) {
			state = CharacterState::ST_Run;
			break;
		}
		break;
	case ST_GrabLedge:
		break;
	}
}

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

	if		(strcmp(anim->name.GetString(),	"DISARMED_IDLE") == 0)	{ disarmed_idle = anim; }
	else if (strcmp(anim->name.GetString(), "DISARMED_RUN")	 == 0)	{ disarmed_run = anim; disarmed_run->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "DISARMED_JUMP") == 0)	{ disarmed_jump = anim; disarmed_jump->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_DJUMP")== 0)	{ disarmed_double_jump = anim; disarmed_double_jump->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_FALL") == 0)	{ disarmed_fall = anim; }
	else if (strcmp(anim->name.GetString(), "DISARMED_MP")	 == 0)	{ disarmed_mp = anim; disarmed_mp->loop = false; }
	else if (strcmp(anim->name.GetString(), "DISARMED_LK")	 == 0)	{ disarmed_lk = anim; disarmed_lk->loop = false; }

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
	}
	else
	{
		player_Collider->rect.x = playerPos.x;
		player_Collider->rect.y = playerPos.y;
	}

}

void j1Player::SetPlayerPosFromCurrentLevel(pugi::xml_node& it)
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
		playerPos.x = it.attribute("value").as_int();
		if (player_Collider != nullptr)
		{
			player_Collider->rect.x = it.attribute("value").as_int();
		}
	}
	if (strcmp(it.attribute("name").as_string(), pos_y) == 0)
	{
		playerPos.y = it.attribute("value").as_int();
		if (player_Collider != nullptr)
		{
			player_Collider->rect.y = it.attribute("value").as_int();
		}
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
	else if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && canDoubleJump && doublejumpCount == 0 && !atCeiling) {
		doubleJumped = true;
	}
}
void j1Player::AttackInputs()
{

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN){
		MP_attackPressed = true;
	}	
	
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN){
		LK_attackPressed = true;
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

void j1Player::HorizontalMove(float dt)
{
	if (moveLeft == moveRight) {
		runSpeed = 0.f;
	}
	else if (moveRight) {
		int posX = 0;
		player_Collider->rect.x += (int)roundf(runSpeed * ceil(dt * 50));
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
		player_Collider->rect.x += (int)roundf(runSpeed * ceil(dt * 50));
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
	if (runSpeed < 0.f) {
		flip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
	}
	else if (runSpeed > 0.f) {
		flip = SDL_RendererFlip::SDL_FLIP_NONE;
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

void j1Player::UpdateColliderSize()
{
	if (state == ST_Fall) {
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
	BROFILER_CATEGORY("DrawLogic", Profiler::Color::Aqua)
	numCurrentAnimation = currentAnimation->GetSprite();

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
	
	App->render->Blit(player_tmx_data.texture, playerPos.x + currentAnimation->offset.x, playerPos.y, &currentAnimation->sprites[numCurrentAnimation].rect,App->render->drawsize,true,flip);
}

#pragma endregion