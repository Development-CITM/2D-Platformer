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

	AABB_current = App->collider->AddCollider({ (int)mPlayerPos.x,(int)mPlayerPos.y,20,50 },COLLIDER_PLAYER);
	currentAnimation = sheathed_idle;

	return true;
}

bool j1Player::CleanUp()
{
	//// Remove all layers
	//p2List_item<ObjectLayer*>* item;
	//item = player_tmx_data.object_Layers.start;

	//while (item != NULL)
	//{
	//	for (int i = 0; i < item->data->rects.count(); i++)
	//	{
	//		delete item->data->rects[i];
	//	}
	//	RELEASE(item->data);
	//	item = item->next;
	//}
	//player_tmx_data.object_Layers.clear();


	// Clean up the pugui tree
	player_file.reset();

	return true;
}



bool j1Player::Load(const char* file_name)
{
	bool ret = true;

	pugi::xml_parse_result result = player_file.load_file(file_name);

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

	if (strcmp(anim->name.GetString(), "SHEATHED_IDLE") == 0) { sheathed_idle = anim; }
	if (strcmp(anim->name.GetString(), "SHEATHED_RUN") == 0) { sheathed_run = anim; }
	if (strcmp(anim->name.GetString(), "SHEATHED_JUMP") == 0) { sheathed_jump = anim; sheathed_jump->loop = false; }

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
		anim->sprites[i].AABB_offset = { anim->sprites[i].AABB_rect.x - (int)roundf(mPlayerPos.x),anim->sprites[i].AABB_rect.y - (int)roundf( mPlayerPos.y) };
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

	rect.x = mPlayerPos.x + offset.x;
	rect.y = mPlayerPos.y + offset.y;

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
				mPlayerPos.x = it.attribute("value").as_int();
			}
			if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
			{
				mPlayerPos.y = it.attribute("value").as_int();
			}
			if (strcmp(it.attribute("name").as_string(), "ground_pos") == 0)
			{
				groundPos = it.attribute("value").as_int();
			}
		}
	}
	
}


bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{

	
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		if (currentAnimation != sheathed_run) {
			currentAnimation->ResetAnim();
			currentAnimation = sheathed_run;
			mState = PlayerState::RUN;
		}
		if (timer < 1.f) {
			timer++;
		}
		else if (mAcceleration < 1.0f) {
			mAcceleration += 0.25f;
			timer = 0.0f;
		}
		if (mAcceleration > 1.f) { mAcceleration = 1.f; }

		mSpeed.x = mRunSpeed * mAcceleration;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		if (currentAnimation != sheathed_idle) {
			currentAnimation->ResetAnim();
			currentAnimation = sheathed_idle;
			mSpeed.x = 0.0f;
			mAcceleration = 0.0f;
			timer = 0.0f;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (currentAnimation != sheathed_jump) {
			sheathed_jump->ResetAnim();
			currentAnimation = sheathed_jump;
			mState = PlayerState::JUMP;
			mSpeed.y = -mJumpSpeed;
		}
	}

	if (mPlayerPos.y >= groundPos) {
		onGround = true;
		if (mState != RUN) {
			currentAnimation = sheathed_idle;
		}
		mPlayerPos.y = groundPos;
	}
	else {
		onGround = false;
		currentAnimation = sheathed_jump;
	}
	if (!onGround) {
		mSpeed.y -= -.3f;

	}

	mPlayerPos += mSpeed;
	Draw(); //Draw all the player
	LOG("Player Pos X: %f", mPlayerPos.x);
	LOG("Player Pos Y: %f", mPlayerPos.y);
	return true;
}



bool j1Player::PostUpdate()
{
	return true;
}



void j1Player::Draw()
{
	//Round Pos beforeblit
	p2Point<int> roundedPos = { (int)roundf(mPlayerPos.x),(int)roundf(mPlayerPos.y) };
	int i = currentAnimation->GetSprite();
	currentAnimation->sprites[i].AABB_rect.x = roundedPos.x + currentAnimation->sprites[i].AABB_offset.x;
	currentAnimation->sprites[i].AABB_rect.y = roundedPos.y + currentAnimation->sprites[i].AABB_offset.y;
	AABB_current->Resize(currentAnimation->sprites[i].AABB_rect);
	App->render->Blit(player_tmx_data.texture, roundedPos.x, roundedPos.y, &currentAnimation->sprites[i].rect,3.f);
}
