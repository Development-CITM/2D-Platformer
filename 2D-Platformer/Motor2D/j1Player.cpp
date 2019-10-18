#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Colliders.h"

#include <math.h>

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
	
	//Init player pos
	playerPos = { 200,472 };

	return ret;
}

bool j1Player::Start()
{
	ground_Collider = App->collider->AddCollider({ 0,28,13,5 }, COLLIDER_GROUND_CHECKER, { 0,0 },this);

	//Load Player tmx (it contains animations and colliders properties)
	Load("animations/Player.tmx");
	state = ST_IDLE;	//Set initial state
	return true;
}

bool j1Player::PreUpdate()
{

	onGround = false;
	SetDetectedCollision(false);
	//Hold Movements
	switch (state)
	{
	case ST_IDLE:
		HoldHorizontalMove();
		break;
	case ST_RUNNING:
		HoldHorizontalMove();
		break;

	}


	Jump();


	currentVelocity.x = playerPos.x - lastPos.x;
	currentVelocity.y = playerPos.y - lastPos.y;
	LOG("Velocity: (%i,%i)", currentVelocity.x, currentVelocity.y);
	//LOG("Previous: (%i,%i)", previousPos.x, previousPos.y);
	LOG("Actual Pos: (%i,%i)", playerPos.x, playerPos.y);
	

	return true;

}

void j1Player::Jump()
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		jumping = true;
		maxVerticalJump.y = playerPos.y - 40;
	}
	if(jumping)
		WantToMove(DIR_UP);

}

void j1Player::ChangeAnimation(Animation* anim)
{
	if (currentAnimation != anim) {
		previousAnimation = currentAnimation;
		previousAnimation->ResetAnim();
		currentAnimation = anim;
	}
}

bool j1Player::Update(float dt)
{
	MoveToPosition();
	
	Draw(); //Draw all the player
	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

void j1Player::Draw()
{
	//First check how many frames should repeat before change its sprite frame
	if (currentAnimation->repeatFrames > 6) { //need to change this 6 to number of frames of each sprite frame -- trying to remove all magic numbers
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
	App->render->Blit(player_tmx_data.spriteSheet.texture,		//Texture loaded from tmx
		playerPos.x,											//Player position.x
		playerPos.y, 											//Player position.y
		&currentAnimation->rects[currentAnimation->numFrame],	//Current rect from animation rects
		App->render->drawsize,									//DrawSize is a multiplier to scale sprites
		flip);													//Orientation to flip sprites
	
	
}

bool j1Player::CleanUp()
{
	
	// Remove all layers
	p2List_item<ObjectLayer*>* item2;
	item2 = player_tmx_data.object_Layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	player_tmx_data.object_Layers.clear();

	// Clean up the pugui tree
	player_file.reset();



	return true;
}

void j1Player::OnCollision(Collider* c1,Collider* c2)
{
	switch (c1->type)
	{
	case COLLIDER_PLAYER:
		switch (c2->type)
		{
		case COLLIDER_WALL_SOLID:
			LOG("WALL SOLID COLLIDED");
			SetDetectedCollision(true);
			break;
		case COLLIDER_WALL_TRASPASSABLE:
			LOG("WALL TRASPASSBLE COLLIDED");
			SetDetectedCollision(true);
			break;
		case COLLIDER_DEAD:
			break;
		}
		break;
	case COLLIDER_GROUND_CHECKER:
		switch (c2->type)
		{
		case COLLIDER_WALL_SOLID:
			onGround = true;
			break;
		case COLLIDER_WALL_TRASPASSABLE:
			LOG("WALL TRASPASSBLE COLLIDED");
			break;

		}
		break;
	default:
		onGround = false;
		break;
	}

}

bool j1Player::Load(const char* file_name)
{
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
		
		ObjectLayer* lay = new ObjectLayer(); //Create new ObjectLayer to create new adress of ObjectLayer type

		ret = LoadLayer(layer, lay); //Layer is a node to layer node, and Lay its the adress of the new ObjectLayer to fill it

		if (ret == true && strcmp(lay->name.GetString(), "Collider") != 0) {//if LoadLayer wents well, it return a true
			player_tmx_data.object_Layers.add(lay);	//Add filled ObjectLayer to the list of ObjectLayers
		}
		else {
			SDL_Rect rect = *lay->rects.start->data;
			p2Point<int> offset{ 0, 0 };
			offset.x = rect.x;
			offset.y = rect.y;
			rect.x += playerPos.x;
			rect.y += playerPos.y;
			player_Collider = App->collider->AddCollider(rect, COLLIDER_PLAYER,offset,this); //Create collider, collider param is the node, playerPos: pos to collider
															//And num 1 (need to change to param, now it sets enum PLAYER collider type)
		}
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", player_tmx_data.width, player_tmx_data.height);
		LOG("tile_width: %d tile_height: %d", player_tmx_data.tile_width, player_tmx_data.tile_height);


		p2List_item<ObjectLayer*>* item_layer = player_tmx_data.object_Layers.start; //Pointer to the first item of object_Layers
		Animation** animations = new Animation*[player_tmx_data.object_Layers.count()]; //Create dynamic array, object_Layers sized
		
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
		
		
		
	}
	RELEASE_ARRAY(animations);
	currentAnimation = idle;
}

void j1Player::HoldHorizontalMove()
{
	//Run forward
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		WantToMove(DIR_RIGHT);
		flip = SDL_FLIP_NONE;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		last_input = IN_IDLE;
	}

	//Run backward
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		WantToMove(DIR_LEFT);
		flip = SDL_FLIP_HORIZONTAL; //Flipped bc it's going to back
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		last_input = IN_IDLE;
	}

	state = ST_RUNNING;
}

// Load map general properties ---------------------------------------------------
bool j1Player::LoadMap()
{
	bool ret = true;
	pugi::xml_node map =  player_file.child("map");

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
void j1Player::MoveToPosition()
{
	if (!GetDetectedCollision()) {
		p2Point<int> newPos{ 0,0 };
		newPos.x = player_Collider->GetPosition().x - player_Collider->offset.x;
		newPos.y = player_Collider->GetPosition().y - player_Collider->offset.y;
		playerPos = newPos;
		
	}
	else {
		
		player_Collider->MoveCollider(lastPos);
		jumping = false;
	}

}
bool j1Player::WantToMove(Directions dir)
{
	bool ret = true;
	lastPos = player_Collider->GetPosition();
	nextPos = player_Collider->GetPosition();
	switch (dir)
	{
	case DIR_RIGHT:
		nextPos.x += forwardVector.x;

		break;
	case DIR_LEFT:
		nextPos.x -= forwardVector.x;
		break;
	case DIR_UP:
		if (playerPos.y > maxVerticalJump.y) {
			nextPos.y += upVector.y;
		}
		break;
	}
	player_Collider->MoveCollider(nextPos);
	return ret;
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
			SDL_Rect* rect = new SDL_Rect;
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
