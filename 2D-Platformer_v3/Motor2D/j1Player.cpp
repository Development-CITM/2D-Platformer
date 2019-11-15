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
	LOG("Loading Player");
	bool ret = true; 

	pugi::xml_node player_values = conf;
	jumpDistance = player_values.child("jumpDistance").attribute("value").as_uint();
	timeOnAir = player_values.child("timeOnAir").attribute("value").as_uint();
	
	max_jumpSpeed = player_values.child("max_jumpSpeed").attribute("value").as_uint();
	max_FallSpeed = player_values.child("max_FallSpeed").attribute("value").as_uint();
	gravityForce = player_values.child("gravityForce").attribute("value").as_uint();
	max_gravityForce = player_values.child("max_gravityForce").attribute("value").as_uint();
	runSpeed = player_values.child("runSpeed").attribute("value").as_uint();
	max_dashSpeed = player_values.child("max_dash_Speed").attribute("value").as_uint();
	dash_distance = player_values.child("dash_distance").attribute("value").as_uint();

	playerheight_dir_down= player_values.child("playerheight_dir_down").attribute("value").as_int();
	colliderheight_dir_down = player_values.child("colliderheight_dir_down").attribute("value").as_int();



	pivot_x_flip = player_values.child("pivot_x_flip").attribute("value").as_int();
	return ret;
	
}

bool j1Player::Start()
{
	//Load Player tmx (it contains animations and colliders properties)
	Load("animations/Player.tmx");

	//Init player pos
	
	state = ST_IDLE;	//Set initial state
	//fallSpeed = max_FallSpeed;
	return true;
}

bool j1Player::CleanUp()
{

	// Remove all layers
	p2List_item<ObjectLayer*>* item2;
	item2 = player_tmx_data.object_Layers.start;

	while (item2 != NULL)
	{
		for (int i = 0; i < item2->data->rects.count(); i++)
		{
			delete item2->data->rects[i];
		}
		RELEASE(item2->data);
		item2 = item2->next;
	}
	player_tmx_data.object_Layers.clear();


	player_Collider = nullptr;


	RELEASE(idle);
	RELEASE(run);
	RELEASE(jump);
	RELEASE(fall);
	
	currentAnimation = nullptr;
	previousAnimation = nullptr;

	RELEASE(rect);
	RELEASE(lay);
	// Clean up the pugui tree
	player_file.reset();

	return true;
}

#pragma endregion


#pragma region Load Info
bool j1Player::Load(const char* file_name)
{

	playerPos = { player_pos_x, player_pos_y };

	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = player_file.load_file(file_name);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}
	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = player_file.child("map").child("objectgroup"); layer && ret; layer = layer.next_sibling("objectgroup"))
	{

		lay = new ObjectLayer(); //Create new ObjectLayer to create new adress of ObjectLayer type
		
		ret = LoadLayer(layer, lay); //Layer is a node to layer node, and Lay its the adress of the new ObjectLayer to fill it
		if (!App->scene->notfirst)
		{
			if (strcmp(layer.attribute("name").as_string(), "Jump_sfx") == 0) {
				pugi::xml_node source = layer.child("object").child("properties").child("property");
				App->audio->LoadFx(source.attribute("value").as_string());
			}
			if (strcmp(layer.attribute("name").as_string(), "Dead_sfx") == 0) {
				pugi::xml_node source = layer.child("object").child("properties").child("property");
				App->audio->LoadFx(source.attribute("value").as_string());
			}
		}
		if (ret == true && strcmp(lay->name.GetString(), "Collider") != 0) {//if LoadLayer went well, it return a true
			player_tmx_data.object_Layers.add(lay);	//Add filled ObjectLayer to the list of ObjectLayers
		}
		else {
			SDL_Rect rect = *lay->rects.start->data;
			p2Point<int> offset{ 0, 0 };
			offset.x = rect.x;
			offset.y = rect.y;
			rect.x += playerPos.x;
			rect.y += playerPos.y;
			player_Collider = App->collider->AddCollider(rect, COLLIDER_PLAYER, offset, this);	

		}
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", player_tmx_data.width, player_tmx_data.height);
		LOG("tile_width: %d tile_height: %d", player_tmx_data.tile_width, player_tmx_data.tile_height);


		p2List_item<ObjectLayer*>* item_layer = player_tmx_data.object_Layers.start; //Pointer to the first item of object_Layers
		Animation** animations = new Animation * [player_tmx_data.object_Layers.count()]; //Create dynamic array, object_Layers sized

		int i = 0; //count to iterate animations
		while (item_layer != NULL)
		{

			ObjectLayer* l = item_layer->data;

			animations[i] = new Animation;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());

			LOG("Rects count: %d", l->rects.count());
			animations[i]->name = l->name.GetString();

			p2List_item<SDL_Rect*>* rect = l->rects.start;
			SDL_Rect* r = rect->data;
			animations[i]->rects = new SDL_Rect[l->rects.count()];
			animations[i]->numRects = l->rects.count();
			for (int j = 0; j < l->rects.count(); j++)
			{
				animations[i]->rects[j] = *l->rects[j];

				LOG("x= %d y= %d width= %d height= %d", r->x, r->y, r->w, r->h);
				if (rect->next != nullptr) {
					rect = rect->next;
					r = rect->data;
				}

			}
			i++;
			item_layer = item_layer->next;
		}

		SetAnimations(animations);
	}
	map_loaded = ret;

	return ret;
}

void j1Player::SetAnimations(Animation** animations) {

	for (int i = 0; i < player_tmx_data.object_Layers.count(); i++)
	{
		SDL_Rect* r = animations[i]->rects;
		p2SString name = animations[i]->name.GetString();

		if (strcmp(name.GetString(), "Idle") == 0) {

			idle = animations[i];
		}
		else if (strcmp(name.GetString(), "Running") == 0) {
			run = animations[i];
		}
		else if (strcmp(name.GetString(), "Jump") == 0) {
			jump = animations[i];
		}
		else if (strcmp(name.GetString(), "Falling") == 0) {
			fall = animations[i];
		}


	}

	RELEASE_ARRAY(animations);
	currentAnimation = idle;
}


// Load map general properties ---------------------------------------------------
bool j1Player::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = player_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		player_tmx_data.width = map.attribute("width").as_int();
		player_tmx_data.height = map.attribute("height").as_int();
		player_tmx_data.tile_width = map.attribute("tilewidth").as_int();
		player_tmx_data.tile_height = map.attribute("tileheight").as_int();

		LoadSpriteSheet(map);

		return ret;
	}
}

bool j1Player::LoadLayer(pugi::xml_node& node, ObjectLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	pugi::xml_node layer_data = node.child("object");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'objectgroup/object' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		for (pugi::xml_node object = layer_data; object; object = object.next_sibling("object"))
		{
			rect = new SDL_Rect;
			rect->x = object.attribute("x").as_int();
			rect->y = object.attribute("y").as_int();
			rect->w = object.attribute("width").as_int();
			rect->h = object.attribute("height").as_int();
			layer->rects.add(rect);
		}
	}

	return ret;
}

bool j1Player::LoadSpriteSheet(pugi::xml_node& node)
{
	bool ret = true;
	pugi::xml_node image = node.child("imagelayer").child("image");
	p2SString source = image.attribute("source").as_string();
	source.create("animations/%s", source.GetString());
	player_tmx_data.spriteSheet.texture = App->tex->Load(source.GetString());
	if (player_tmx_data.spriteSheet.texture != nullptr) {
		LOG("Texture loaded successfull");
	}

	player_tmx_data.spriteSheet.width = image.attribute("width").as_uint();
	player_tmx_data.spriteSheet.height = image.attribute("height").as_uint();

	LOG("Texture source: %s", source.GetString());
	LOG("Texture width: %d", player_tmx_data.spriteSheet.width);
	LOG("Texture height: %d", player_tmx_data.spriteSheet.height);

	return ret;
}

#pragma endregion

#pragma region Pre/Update/Post

bool j1Player::PreUpdate()
{
	velocity_X = playerPos.x;
	velocity_Y = playerPos.y;

	if (!god_mode_enabled && state != ST_DASH)
	{
		Gravity();
	}


	//Hold Movements
	if (canMove) {
		switch (state)
		{
		case ST_IDLE:
			ExitInput();
			VerticalInput();
			HorizontalInput();
			break;
		case ST_RUNNING:
//			DashInput();
			ExitInput();
			VerticalInput();
			HorizontalInput();
			break;
		case ST_JUMP:
			DashInput();
			HorizontalInput();
			break;
		case ST_FALL:
			DashInput();
			HorizontalInput();

			break;
		}
	}
	return true;
}

bool j1Player::Update(float dt)
{	
	if (!god_mode_enabled)
	{
		Move();
		Jump();
		Dash();
	}
	else if (god_mode_enabled)
	{
		MoveOnGodMode();
	}

	velocity_X -= playerPos.x;
	velocity_Y -= playerPos.y;
	
	//Change Animations

	if (velocity_X == 0 && velocity_Y == 0 && !jumping && state != ST_JUMP && state != ST_FALL) {
		ChangeAnimation(idle);
		state = ST_IDLE;
	}

	if (velocity_X != 0  && velocity_Y == 0 && state != ST_JUMP && state != ST_FALL) {
		if (!dash) {
			ChangeAnimation(run);
			state = ST_RUNNING;
		}
		else {
			ChangeAnimation(jump);
		}
	}

	if (velocity_Y > 0) {
		ChangeAnimation(jump);		
		state = ST_JUMP;
	}

	if (velocity_Y < 0 && state != ST_IDLE) {
		if(currentAnimation != fall)
		ChangeAnimation(fall);	
		if (state != ST_FALL) {
			state = ST_FALL;
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

	LOG("COLLIDER: x: %i y: %i", player_Collider->rect.x, player_Collider->rect.y);
	LOG("PLAYER: x: %i y: %i", playerPos.x, playerPos.y);
	Draw(); //Draw all the player

	return true;
}



bool j1Player::PostUpdate()
{
	return true;
}

#pragma endregion

#pragma region Inputs

void j1Player::VerticalInput()
{
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && onGround) {
		App->audio->PlayFx(2);
		maxJump = player_Collider->rect.y - jumpDistance;
		jumping = true;
		jumpSpeed = max_jumpSpeed;
		gravityForce = 2;
		state = ST_JUMP;
	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN && onPlatform) {
		MoveTo(DIR_PLATFORM);
	}
}


void j1Player::SetPlayerPos(pugi::xml_node& node)
{
	for (pugi::xml_node it = node.child("properties").child("property"); it; it = it.next_sibling("property"))
	{
		if (strcmp(it.attribute("name").as_string(), "player_pos_x") == 0)
		{
			player_pos_x = it.attribute("value").as_int();
		}
		if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
		{
			player_pos_y = it.attribute("value").as_int();
		}
	}
}

void j1Player::DashInput()
{
	if (App->input->GetKey(SDL_SCANCODE_W)== KEY_DOWN && canDash) {
		dash = true;
		canDash = false;
		state = ST_DASH;
		dashSpeed = max_dashSpeed;
		switch (last_Direction)
		{
		case DIR_RIGHT:
			max_Dash = player_Collider->rect.x + dash_distance;
			break;
		case DIR_LEFT:
			max_Dash = player_Collider->rect.x - dash_distance;
			break;
		}
		
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

	if(App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT){
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


#pragma endregion

#pragma region MoveLogic


bool j1Player::MoveTo(Directions dir)
{
	bool ret = false;

	previousColliderPos = player_Collider->GetPosition();
	previousPlayerPos = playerPos;

		switch (dir)
		{
		case DIR_DASH_LEFT:
			player_Collider->rect.x -= dashSpeed;
			playerPos.x -= dashSpeed;
			if (App->collider->CheckColliderCollision(player_Collider)) {
				player_Collider->rect.x = previousColliderPos.x;
				playerPos.x = previousPlayerPos.x;
				state = ST_FALL;
				dash = false;
			}
			else if (relativePos.x < 180 && App->tiles->culling_Collider->rect.x > App->scene->camera_limit_left) {
				App->render->camera.x += dashSpeed * 2;

				App->tiles->culling_Collider->rect.x -= dashSpeed;
			}
			break;

		case DIR_DASH_RIGHT:
			player_Collider->rect.x += dashSpeed;
			playerPos.x += dashSpeed;
			if (App->collider->CheckColliderCollision(player_Collider)) {
				player_Collider->rect.x = previousColliderPos.x;
				playerPos.x = previousPlayerPos.x;
				state = ST_FALL;
				dash = false;
			}
			else if (relativePos.x > 220 && App->tiles->culling_Collider->rect.x < App->scene->camera_limit_right) {
				App->render->camera.x -= dashSpeed * 2;
				App->tiles->culling_Collider->rect.x += dashSpeed;

			}
			break;

		case DIR_RIGHT:
			player_Collider->rect.x += runSpeed;
			playerPos.x += runSpeed;
			if (App->collider->CheckColliderCollision(player_Collider)) {
				player_Collider->rect.x = previousColliderPos.x;
				playerPos.x = previousPlayerPos.x;
			}
			else if (relativePos.x > 220 && App->tiles->culling_Collider->rect.x < App->scene->camera_limit_right) {
				App->render->camera.x -= runSpeed * 2;
				App->tiles->culling_Collider->rect.x += runSpeed;

			}
			break;
		case DIR_LEFT:
			player_Collider->rect.x -= runSpeed;
			playerPos.x -= runSpeed;
			if (App->collider->CheckColliderCollision(player_Collider) || relativePos.x < 6) {
				player_Collider->rect.x = previousColliderPos.x;
				playerPos.x = previousPlayerPos.x;
			}
			else if (relativePos.x < 180 && App->tiles->culling_Collider->rect.x > App->scene->camera_limit_left) {
				App->render->camera.x += runSpeed * 2;

				App->tiles->culling_Collider->rect.x -= runSpeed;
			}
			break;
		case DIR_UP:
			player_Collider->rect.y -= jumpSpeed;
			playerPos.y -= jumpSpeed;

			if (App->collider->CheckColliderCollision(player_Collider)) {
				player_Collider->rect.y = previousColliderPos.y;
				playerPos.y = previousPlayerPos.y;
				jumping = false;
				state = ST_FALL;
				gravityForce = 1;
				currentTimeAir = 0;
			}
			else if (relativePos.y < 100 && App->tiles->culling_Collider->rect.y > 10) {
				App->render->camera.y += 6;

				App->tiles->culling_Collider->rect.y -= 3;
			}


			break;
		case DIR_PLATFORM:
			player_Collider->rect.y += 10;
			playerPos.y += 10;

			if (App->collider->ThroughPlatform(player_Collider) == false) {
				state = ST_FALL;
			}

			break;

		case DIR_DOWN:
			int offsetY = 0;
			player_Collider->rect.y += gravityForce;
			playerPos.y += gravityForce;

			if (App->collider->CheckColliderCollision(player_Collider, &offsetY)) {
				player_Collider->rect.y = previousColliderPos.y;
				player_Collider->rect.y = offsetY - colliderheight_dir_down;
				playerPos.y = previousPlayerPos.y;
				playerPos.y = offsetY - playerheight_dir_down;
				onGround = true;
				canDash = true;
				state = ST_IDLE;
				ret = true;
				if (relativePos.y > 330) {
					App->render->camera.y -= 8;

					App->tiles->culling_Collider->rect.y += 8 / 2;
				}
			}
			else if (relativePos.y > 230 && App->tiles->culling_Collider->rect.y < 250 && state != ST_JUMP) {
				int cameraSpeedY = 8;
				if (gravityForce == max_gravityForce) {
					cameraSpeedY = max_gravityForce;
				}
				else {
					cameraSpeedY = 8;
				}
				App->render->camera.y -= cameraSpeedY;
				App->tiles->culling_Collider->rect.y += cameraSpeedY / 2; 
			}

			else {
				onGround = false;
				//LOG("Gravity: %i", gravityForce);
			}

			break;
		
	}

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
			state = ST_FALL;
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
	else if(gravityForce < max_gravityForce){
		currentTimeAir = 0;
		gravityForce += 2;
	}
	else if (gravityForce > max_gravityForce){
		gravityForce = max_gravityForce;
	}

}

void j1Player::MoveOnGodMode()
{
	Directions dir = DIR_NONE;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		dir = DIR_RIGHT;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		dir = DIR_UP;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		dir = DIR_DOWN;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		dir = DIR_LEFT;

	switch (dir)
	{
	case DIR_RIGHT:
		player_Collider->rect.x += runSpeed*2;
		playerPos.x += runSpeed*2;
		App->render->camera.x -= runSpeed * 4;
		App->tiles->culling_Collider->rect.x += runSpeed*2;
		break;
	case DIR_LEFT:
		player_Collider->rect.x -= runSpeed*2;
		playerPos.x -= runSpeed*2;
		App->render->camera.x += runSpeed * 4;
		App->tiles->culling_Collider->rect.x -= runSpeed*2;
		break;
	case DIR_UP:
		player_Collider->rect.y -= runSpeed*2;
		playerPos.y -= runSpeed*2;

		App->render->camera.y += runSpeed*4;

		App->tiles->culling_Collider->rect.y -= runSpeed*2;
		break;
	case DIR_DOWN:
		player_Collider->rect.y += runSpeed*2;
		playerPos.y += runSpeed*2;

		App->render->camera.y -= runSpeed*4;
		App->tiles->culling_Collider->rect.y += runSpeed*2;

		break;
	}
}

void j1Player::Dash() {

	if (dash) {
		switch (last_Direction)
		{
		case DIR_RIGHT:
			if (player_Collider->rect.x <= max_Dash) {
				MoveTo(DIR_DASH_RIGHT);

				if (player_Collider->rect.x > max_Dash - 30) {
					dashSpeed = 3;	  
				}						  
				if (player_Collider->rect.x > max_Dash - 5) {
					dashSpeed = 1;	  
				}						  
			}
			else {
				dash = false;
			}
			
			break;
		case DIR_LEFT:
			if (player_Collider->rect.x >= max_Dash) {
				MoveTo(DIR_DASH_LEFT);

				if (player_Collider->rect.x < max_Dash - 30) {
					dashSpeed = 3;
				}
				if (player_Collider->rect.x < max_Dash - 5) {
					dashSpeed = 1;
				}
			}
			else {
				dash = false;
			}
			break;
		}
	}
}

#pragma endregion

#pragma region Render

void j1Player::Draw()
{
	if (currentAnimation != nullptr) {
		//First check how many frames should repeat before change its sprite frame
		if (currentAnimation->repeatFrames > 4) { //need to change this 6 to number of frames of each sprite frame -- trying to remove all magic numbers
			if (currentAnimation->numFrame < currentAnimation->numRects - 1) //Check if you reach the last frame of the animations (-1 is bc you don't want to go out of the array)
			{
				currentAnimation->numFrame++;
			}
			else {
				currentAnimation->numFrame = 0;
			}

			currentAnimation->repeatFrames = 0; //Reset repeat frames to start again the count of the same sprite. but this should be a function (maybe)

		}
		else {
			currentAnimation->repeatFrames++; // Increse num of frames before change its animations sprite
		}
	}
	if (currentAnimation != nullptr) {
		App->render->Blit(player_tmx_data.spriteSheet.texture,		//Texture loaded from tmx
			playerPos.x,											//Player position.x
			playerPos.y, 											//Player position.y
			&currentAnimation->rects[currentAnimation->numFrame],	//Current rect from animation rects
			App->render->drawsize,
			false,							//DrawSize is a multiplier to scale sprites
			flip);
	}							//Orientation to flip sprites
	
	
}


#pragma endregion

#pragma region Animations
void j1Player::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim && currentAnimation != nullptr) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}

#pragma endregion

