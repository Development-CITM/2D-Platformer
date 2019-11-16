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

#pragma region Constructor/Awake/Start/CleanUp

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

	pivot_x_flip = player_values.child("pivot_x_flip").attribute("value").as_int();
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



bool j1Player::Load(const char* file_name)
{
	bool ret = true;
	pugi::xml_document	player_file;
	pugi::xml_parse_result result = player_file.load_file(file_name);

	player_Collider = App->collider->AddCollider({ playerPos.x,playerPos.y,20,45 }, COLLIDER_PLAYER, { 0,0 }, this);
	ceilingChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,player_Collider->rect.w ,3 }, COLLIDER_CEILING_CHECKER, { 0,-3 }, this);
	rightChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,4,player_Collider->rect.h}, COLLIDER_CEILING_CHECKER, { player_Collider->rect.w,0 }, this);
	leftChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,4,player_Collider->rect.h}, COLLIDER_CEILING_CHECKER, { -3,0 }, this);
	groundChecker = App->collider->AddCollider({ playerPos.x,playerPos.y,player_Collider->rect.w,5}, COLLIDER_CEILING_CHECKER, { 0,player_Collider->rect.h }, this);

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
			}
			if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
			{
				playerPos.y = it.attribute("value").as_int();
			}
			if (strcmp(it.attribute("name").as_string(), "ground_pos") == 0)
			{
				groundPos = it.attribute("value").as_int();
			}
		}
	}
	
}

void j1Player::OnCollision(Collider* c1, Collider* c2)
{
	
}


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
		last_Direction = direction;
		direction = Directions::DIR_RIGHT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		moveRight = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		moveLeft = true;
		last_Direction = direction;
		direction = Directions::DIR_LEFT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		moveLeft = false;
		//direction = DIR_UP;
	}
}

bool j1Player::Update(float dt)
{
	
//---------NEED TO-------//
//UpdateState depending of inputs
//Gravity affects allways and its independent from 
//Iterate from all states and execute the logic from currentState
//Animations should be declared here once you changed the currentState
//What happen on every state? (sperate each logic state on functions)
//Example: OnRunState:Call function to move right or left: 
//AABB Collider should be diffent from each animation (would be better a differnt collider from each animation frame, but for now, simplify)
//this function : you should move the player (and update its collider at the same time), before its move, check if can move(if any collision would be in the next move)
//If no collision detected, keep going or reach the movement, if not, you should snap your collider nexto the collided one. ( this should be AXIS independent to have more control and avoid conflicts of positions)
//Once you finish this function it should be the position correctly to draw it (round position to int to print) AABB should move on player's position, so it should be rounded by default bc playerposition would be rounded)
//Here, currentstate keeps updating if there are more functions appart of move logic.
//Finish this loop state.
//Now Draw current animation with updated position and AABB position

	velocity_X -= playerPos.x;
	velocity_Y -= playerPos.y;

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


	

	//HorizontalMove();
	//JumpMove();
	LOG("Left: %i Right: %i", moveLeft, moveRight);
	LOG("State: %i", state);
	LOG("Anim: %s", currentAnimation->name.GetString());
	LOG("OnGround: %i", onGround);

	

	//if (verticalSpeed <= 0) {
	//	ChangeAnimation(disarmed_jump);
	//	state = CharacterState::ST_Jump;
	//}
	//else if (verticalSpeed > 0 && !onGround) {
	//	ChangeAnimation(disarmed_fall);
	//	state = CharacterState::ST_Fall;
	//}

	numCurrentAnimation = currentAnimation->GetSprite();
	

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
	
	if (verticalSpeed < 6.f) {
		verticalSpeed += gravitySpeed;
	}

	if (jumpPressed) {
		verticalSpeed = -10;
		state = CharacterState::ST_Jump;
		jumpPressed = false;
		onGround = false;
		jumping = true;
	}
	if (verticalSpeed >= 0.f) {
		jumping = false;
	}
	//Gravity
	if (!onGround && !jumping) {
		int posY = 0;
		player_Collider->rect.y += verticalSpeed;
		if (App->collider->CheckColliderCollision(groundChecker, Directions::DIR_DOWN, &posY)) {
			player_Collider->rect.y = posY;
			onGround = true;
		}
	}
	else if(!onGround) {
		player_Collider->rect.y += verticalSpeed;
	}
	
	if (moveLeft == moveRight) {
		runSpeed = 0.f;
	}
	else if (moveRight) {
		int posX = 0;
		player_Collider->rect.x += runSpeed;
		if (App->collider->CheckColliderCollision(rightChecker,Directions::DIR_RIGHT,&posX)) {
			if (posX != 0) {
			runSpeed = 0.f;
				player_Collider->rect.x = posX;
			}
		}
		else {
			runSpeed = 2.f;
		}
	}
	else if (moveLeft) {
		int posX = 0;
		player_Collider->rect.x += runSpeed;
		if (App->collider->CheckColliderCollision(leftChecker,Directions::DIR_LEFT,&posX)) {
			if (posX != 0) {
				runSpeed = 0.f;
				player_Collider->rect.x = posX;
			}
		}
		else {
			runSpeed = -2.f;
		}
	}


	//App->collider->CheckColliderCollision(player_Collider, {(int) roundf(runSpeed),0 });
	//if (App->collider->CheckColliderCollision(player_Collider, { 0,(int)roundf(verticalSpeed) }, &posX, &posY)) {
	//	if (verticalSpeed > 0) {
	//		onGround = true;
	//	}
	//		verticalSpeed = 0.f;
	//}
	//else {
	//	onGround = false;
	//}
	//if (App->collider->CheckColliderCollision(ceilingChecker)) {
	//	atCeiling = true;
	//}
	//else {
	//	atCeiling = false;
	//}
	LOG("State: %i", state);



	playerPos.x = player_Collider->rect.x - (int) roundf( player_tmx_data.tile_width*0.5) - 2;
	playerPos.y = player_Collider->rect.y - colliderOffsetY1;

	ceilingChecker->rect.x = player_Collider->rect.x + ceilingChecker->offset.x;
	ceilingChecker->rect.y = player_Collider->rect.y + ceilingChecker->offset.y;

	groundChecker->rect.x = player_Collider->rect.x + groundChecker->offset.x;
	groundChecker->rect.y = player_Collider->rect.y + groundChecker->offset.y;

	leftChecker->rect.x = player_Collider->rect.x + leftChecker->offset.x;
	leftChecker->rect.y = player_Collider->rect.y + leftChecker->offset.y;

	rightChecker->rect.x = player_Collider->rect.x + rightChecker->offset.x;
	rightChecker->rect.y = player_Collider->rect.y + rightChecker->offset.y;

	//ReSizeAABBFromAnimation();
	Draw(); //Draw all the player

	return true;
}

void j1Player::JumpMove()
{
	if (jumpPressed) {
		jumping = true;
		verticalSpeed = -10.f;
		jumpPressed = false;
	}
	if (verticalSpeed < 6) {
		verticalSpeed += gravitySpeed;
	}
}

void j1Player::HorizontalMove()
{
	if (moveLeft == moveRight) {
		runSpeed = 0.f;
	}
	else if (moveLeft) {
		runSpeed = -2.f;
	}
	else if (moveRight) {
			runSpeed = 2.f;
	}
}

//void j1Player::ReSizeAABBFromAnimation()
//{
//
//
//	SDL_Rect r;
//	r.x = playerPos.x + currentAnimation->sprites[numCurrentAnimation].AABB_offset.x + 8;
//	r.y = playerPos.y + currentAnimation->sprites[numCurrentAnimation].AABB_offset.y +3;
//	r.w = currentAnimation->sprites[numCurrentAnimation].AABB_rect.w + 10;
//	r.h = currentAnimation->sprites[numCurrentAnimation].AABB_rect.h + 15;
//	player_Collider->Resize(r);
//}





bool j1Player::PostUpdate()
{
	return true;
}


void j1Player::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}




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
