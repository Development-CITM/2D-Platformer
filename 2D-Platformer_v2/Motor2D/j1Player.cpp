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

	player_Collider = App->collider->AddCollider({ playerPos.x,playerPos.y,20,50 }, COLLIDER_PLAYER, { 0,0 }, this);

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
	else if (strcmp(anim->name.GetString(), "DISARMED_RUN")	== 0) {	disarmed_run = anim; }
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
	Gravity();
	//Hold Movements
	if (canMove) {
		switch (state)
		{
		case ST_Idle:
			ExitInput();
			VerticalInput();
			HorizontalInput();
			break;
		case ST_Run:
			//			DashInput();
			ExitInput();
			VerticalInput();
			HorizontalInput();
			break;
		case ST_Jump:
			//DashInput();
			HorizontalInput();
			break;
		case ST_Fall:
			//DashInput();
			HorizontalInput();

			break;
		}
	}
	return true;
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
	//Change Animations
	Move();
	Jump();
	//Dash();
	if (velocity_X == 0 && velocity_Y == 0 && !jumping && state != ST_Jump && state != ST_Fall) {
		ChangeAnimation(disarmed_idle);
		state = ST_Idle;
	}

	//if (velocity_X != 0 && velocity_Y == 0 && state != ST_Jump&& state != ST_Fall) {
	//	if (!dash) {
	//		ChangeAnimation(run);
	//		state = ST_Run;
	//	}
	//	else {
	//		ChangeAnimation(jump);
	//	}
	//}

	if (velocity_Y > 0) {
		ChangeAnimation(disarmed_jump);
		state = ST_Jump;
	}

	if (velocity_Y < 0 && state != ST_Idle) {
		if (currentAnimation != disarmed_fall)
			ChangeAnimation(disarmed_fall);
		if (state != ST_Fall) {
			state = ST_Fall;
			gravityForce = 3;
		}
	}


	if (App->tiles->culling_Collider->CheckCollision(player_Collider->rect)) {
		//LOG("Player_Collider.x: %i", player_Collider->rect.x);
		//LOG("Camera collider.x: %i", App->tiles->camera_Collider->rect.x);

		relativePos.x = player_Collider->rect.x - App->tiles->culling_Collider->rect.x;
		relativePos.y = player_Collider->rect.y - App->tiles->culling_Collider->rect.y;
		//LOG("Relative Pos X: %i  Y: %i", relativePos.x, relativePos.y);
	}

	LOG("State: %i", state);


	numCurrentAnimation = currentAnimation->GetSprite();
	ReSizeAABBFromAnimation();
	Draw(); //Draw all the player

	return true;
}

void j1Player::ReSizeAABBFromAnimation()
{


	SDL_Rect r;
	r.x = playerPos.x + currentAnimation->sprites[numCurrentAnimation].AABB_offset.x + 8;
	r.y = playerPos.y + currentAnimation->sprites[numCurrentAnimation].AABB_offset.y +3;
	r.w = currentAnimation->sprites[numCurrentAnimation].AABB_rect.w + 10;
	r.h = currentAnimation->sprites[numCurrentAnimation].AABB_rect.h + 15;
	player_Collider->Resize(r);
}





bool j1Player::PostUpdate()
{
	return true;
}
void j1Player::VerticalInput()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && onGround) {
		App->audio->PlayFx(2);
		maxJump = player_Collider->rect.y - jumpDistance;
		jumping = true;
		jumpSpeed = max_jumpSpeed;
		gravityForce = 2;
		state = ST_Jump;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && onPlatform) {
		MoveTo(DIR_PLATFORM);
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
void j1Player::ExitInput()
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && App->scene->exitCollider != nullptr) {
		if (player_Collider->CheckCollision(App->scene->exitCollider->rect)) {
			LOG("COLLIDER EXIT DONE");
			App->scene->hasExit = true;
		}
	}
}

void j1Player::HorizontalInput()
{
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		move_To_Right = true;
		last_Direction = DIR_RIGHT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		move_To_Right = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		move_To_Left = true;
		last_Direction = DIR_LEFT;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		move_To_Left = false;
	}

}

void j1Player::ResetInputs()
{
	move_To_Right = false;
	move_To_Left = false;
	move_To_Up = false;

	jumping = false;
	flip = SDL_FLIP_NONE;
}

bool j1Player::MoveTo(Directions dir)
{
	bool ret = false;

	previousColliderPos = player_Collider->GetPosition();
	previousPlayerPos = playerPos;



	
	//("Player: %i,%i", playerPos.x, playerPos.y);
	return ret;

}
void j1Player::Move() {
	if (move_To_Left) {
		MoveTo(DIR_LEFT);
		flip = SDL_FLIP_HORIZONTAL;


	}
	else if (move_To_Right) {
		MoveTo(DIR_RIGHT);
		flip = SDL_FLIP_NONE;
	}
}

void j1Player::Jump()
{
	if (jumping) {
		if (player_Collider->rect.y >= maxJump) {
			MoveTo(DIR_UP);

			if (player_Collider->rect.y < maxJump + PLAYERMAXJUMP_GRAVITY4) {
				gravityForce = 4;
			}
			if (player_Collider->rect.y < maxJump + PLAYERMAXJUMP_GRAVITY6) {
				gravityForce = 6;
			}
			if (player_Collider->rect.y < maxJump + PLAYERMAXJUMP_GRAVITY9) {
				gravityForce = 9;
			}

			onGround = false;
		}
		else {
			jumping = false;
			jumpSpeed = 0;
			state = ST_Fall;
			gravityForce = 3;
			currentTimeAir = 0;
		}
	}
}

void j1Player::Gravity()
{
	MoveTo(DIR_DOWN);
	if (currentTimeAir < timeOnAir) {
		currentTimeAir++;
	}
	else if (gravityForce < max_gravityForce) {
		currentTimeAir = 0;
		gravityForce += 2;
	}
	else if (gravityForce > max_gravityForce) {
		gravityForce = max_gravityForce;
	}

}

void j1Player::Draw()
{
	App->render->Blit(player_tmx_data.texture, playerPos.x, playerPos.y, &currentAnimation->sprites[numCurrentAnimation].rect,2.f,true,flip);
}
