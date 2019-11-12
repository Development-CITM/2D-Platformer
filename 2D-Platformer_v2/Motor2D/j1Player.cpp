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


	mAnimation = disarmed_idle;

	

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

	AABB_current = App->collider->AddCollider({ (int)mPosition.x,(int)mPosition.y,20,50 }, COLLIDER_PLAYER, { 0,0 }, this);

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
	//if (strcmp(anim->name.GetString(), "DISARMED_RUN")	== 0) {	disarmed_run = anim; }
	//if (strcmp(anim->name.GetString(), "DISARMED_JUMP") == 0) { disarmed_jump = anim; disarmed_jump->loop = false; }
	//if (strcmp(anim->name.GetString(), "DISARMED_FALL") == 0) { disarmed_fall = anim; }

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
		anim->sprites[i].AABB_offset = { anim->sprites[i].AABB_rect.x - (int)roundf(mPosition.x),anim->sprites[i].AABB_rect.y - (int)roundf( mPosition.y) };
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

	rect.x = mPosition.x + offset.x;
	rect.y = mPosition.y + offset.y;

	return rect;

}

void j1Player::UpdatePhysics()
{
	mOldPosition = mPosition;
	mOldSpeed = mSpeed;

	mWasOnGround = mOnGround;
	mPushedRightWall = mPushesRightWall;
	mPushedLeftWall = mPushesLeftWall;
	mWasAtCeiling = mAtCeiling;

	mPosition += mSpeed;

	numCurrentAnimation = mAnimation->GetSprite();
	p2Point<int> roundedPos = { (int)roundf(mPosition.x),(int)roundf(mPosition.y) };
	if (flip == SDL_FLIP_NONE) {
		mAnimation->sprites[numCurrentAnimation].AABB_rect.x = roundedPos.x + mAnimation->sprites[numCurrentAnimation].AABB_offset.x;
		mAnimation->sprites[numCurrentAnimation].AABB_rect.y = roundedPos.y + mAnimation->sprites[numCurrentAnimation].AABB_offset.y;
	}
	else {
		mAnimation->sprites[numCurrentAnimation].AABB_rect.x = roundedPos.x - mAnimation->sprites[numCurrentAnimation].AABB_offset.x + player_tmx_data.width *2 -3;
		mAnimation->sprites[numCurrentAnimation].AABB_rect.y = roundedPos.y + mAnimation->sprites[numCurrentAnimation].AABB_offset.y;
	}

	AABB_current->Resize(mAnimation->sprites[numCurrentAnimation].AABB_rect);

	if (mPosition.y >= (float)groundPos)
	{
		mPosition.y = (float)groundPos;
		mOnGround = true;
	}
	else
		mOnGround = false;
}

void j1Player::CharacterUpdate()
{
	switch (mCurrentState)
	{
	case CharacterState::Idle:
		if (StateIdle()) break;
		break;

	case CharacterState::Walk:
		break;
	case CharacterState::Run:
		if (StateRun()) break;
		break;

	case CharacterState::Jump:
		if (StateJump()) break;
		break;

	case CharacterState::Fall:
		if (StateFall()) break;
		break;

	case CharacterState::GrabLedge:
		break;
	}

	LOG("%i", mCurrentState);
}

//State Machine

bool j1Player::StateIdle()
{
	mSpeed.SetToZero();
	if (mAnimation != disarmed_idle) {
		mAnimation->ResetAnim();
		mAnimation = disarmed_idle;
	}

	if (!mOnGround)
	{
		mCurrentState = CharacterState::Fall;
		return true;
	}
	if (mGoLeft != mGoRight && !mJump)
	{
		mCurrentState = CharacterState::Run;
		return true;
	}
	else if (mJump)
	{
		mCurrentState = CharacterState::Jump;
		return true;
	}
	return true;
}

bool j1Player::StateRun()
{
	if (mAnimation != disarmed_run) {
		mAnimation->ResetAnim();
		mAnimation = disarmed_run;
	}

	if (mGoLeft == mGoRight) {
		mCurrentState = CharacterState::Idle;
		mSpeed.SetToZero();
		mRunAcceleration = 0.0f;
		timer = 0.0f;
	}
	else if (mGoRight) {
		if (mPushesRightWall) {
			mSpeed.x = 0.f;
			mRunAcceleration = 0.0f;
			timer = 0.0f;
		}
		else {
			if (timer < 1.f) {
				timer++;
			}
			else if (mRunAcceleration < 1.0f) {
				mRunAcceleration += 0.25f;
				timer = 0.0f;
			}
			if (mRunAcceleration > 1.f) { mRunAcceleration = 1.f; }

			mSpeed.x = mRunSpeed * mRunAcceleration;
		}
		flip = SDL_FLIP_NONE;
	}
	else if (mGoLeft) {
		if (mPushesLeftWall) {
			mSpeed.x = 0.f;
			mRunAcceleration = 0.0f;
			timer = 0.0f;
		}
		else {
			if (timer < 1.f) {
				timer++;
			}
			else if (mRunAcceleration < 1.0f) {
				mRunAcceleration += 0.25f;
				timer = 0.0f;
			}
			if (mRunAcceleration > 1.f) { mRunAcceleration = 1.f; }

			mSpeed.x = -mRunSpeed * mRunAcceleration;
		}

		flip = SDL_FLIP_HORIZONTAL;
	}

	if (mJump)
	{
		mCurrentState = CharacterState::Jump;
		return true;
	}
	else if (!mOnGround) {
		mCurrentState = CharacterState::Fall;
		return true;
	}
	return true;
}

bool j1Player::StateJump()
{
	if (mAnimation != disarmed_jump) {
		mAnimation->ResetAnim();
		mAnimation = disarmed_jump;
		delayToJump = 0.f;
		startJump = false;
	}
	if (delayToJump < 5.f)
	{
		delayToJump++;
		return true;	
	}

	if (delayToJump >= 5.f) {
		if (!startJump) {
			mSpeed.y = mJumpSpeed;
			startJump = true;
		}
		if (mSpeed.y > -0.5f && mSpeed.y < 0.3f) {
			mSpeed.y += 0.1f;
		}
		else if (mSpeed.y < 0.5f) {
			mSpeed.y += cGravity;
		}
		else {
			mCurrentState = CharacterState::Fall;
		}
	}
	HorizontalMove();
	return true;
}

bool j1Player::StateFall()
{
	if (mAnimation != disarmed_fall) {
		mAnimation->ResetAnim();
		mAnimation = disarmed_fall;
	} //Just for now
	if (mSpeed.y < 6.f)
		mSpeed.y += cGravity;

	if (mOnGround) {
		if (mGoLeft == mGoRight) {
			mCurrentState = CharacterState::Idle;
		}
		else {
			mCurrentState = CharacterState::Run;
		}
		mJump = false;
		return true;
	}
	HorizontalMove();
	return true;
}

void j1Player::HorizontalMove()
{
	if (mGoLeft == mGoRight) {

		mSpeed.x = 0.f;
	}
	else if (mGoRight) {
		if (mPushesRightWall) {
			mSpeed.x = 0.f;
			mRunAcceleration = 0.0f;
			timer = 0.0f;
		}
		else {
			if (timer < 1.f) {
				timer++;
			}
			else if (mRunAcceleration < 1.0f) {
				mRunAcceleration += 0.25f;
				timer = 0.0f;
			}
			if (mRunAcceleration > 1.f) { mRunAcceleration = 1.f; }

			mSpeed.x = mRunSpeed * mRunAcceleration;
		
		}
		flip = SDL_FLIP_NONE;
	}
	else if (mGoLeft) {


		if (mPushesLeftWall) {
			mSpeed.x = 0.f;
			mRunAcceleration = 0.0f;
			timer = 0.0f;
		}
		else {
			if (timer < 1.f) {
				timer++;
			}
			else if (mRunAcceleration < 1.0f) {
				mRunAcceleration += 0.25f;
				timer = 0.0f;
			}
			if (mRunAcceleration > 1.f) { mRunAcceleration = 1.f; }

			mSpeed.x = -mRunSpeed * mRunAcceleration;
		}
		flip = SDL_FLIP_HORIZONTAL;
	}
}

void j1Player::CheckCollision()
{
	if (AABB_current != nullptr) {
		LOG("X:%i Y:%i", AABB_current->rect.x, AABB_current->rect.y);
		App->collider->CheckColliderCollision(AABB_current);
	}
}


void j1Player::SetPlayerPos(pugi::xml_node& object)
{
	if (App->scene->loading == false)
	{
		for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) 
		{
			if (strcmp(it.attribute("name").as_string(), "player_pos_x") == 0)
			{
				mPosition.x = it.attribute("value").as_int();
			}
			if (strcmp(it.attribute("name").as_string(), "player_pos_y") == 0)
			{
				mPosition.y = it.attribute("value").as_int();
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

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		mGoRight = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		mGoRight = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		mGoLeft = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP) {
		mGoLeft = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		mJump = true;
	}
	return true;
}

bool j1Player::Update(float dt)
{
	CheckCollision();
	CharacterUpdate();
	UpdatePhysics();
	Draw(); //Draw all the player
	LOG("Player Pos X: %f", mPosition.x);
	LOG("Player Pos Y: %f", mPosition.y);
	return true;
}



bool j1Player::PostUpdate()
{
	return true;
}



void j1Player::Draw()
{
	App->render->Blit(player_tmx_data.texture, (int)roundf(mPosition.x), (int)roundf(mPosition.y), &mAnimation->sprites[numCurrentAnimation].rect,2.f,false,flip);
}
