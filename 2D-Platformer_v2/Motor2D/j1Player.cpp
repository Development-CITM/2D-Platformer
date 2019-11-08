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

	player_pos = { 100,422 };
	Load("animations/Player.tmx");

	AABB_current = AABB_sheathed_idle;
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
		LoadAABB(player_node);

		pugi::xml_node group = player_node.child("group");

		for (group; group && ret; group = group.next_sibling("group"))
		{
			for (pugi::xml_node object_group = group.child("objectgroup"); object_group && ret; object_group = object_group.next_sibling("objectgroup"))
			{
				LoadAnimation(object_group);
			}
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


void j1Player::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "SHEATHED_IDLE") == 0) { sheathed_idle = anim; }
	if (strcmp(anim->name.GetString(), "SHEATHED_RUN") == 0) { sheathed_run = anim; }

	anim->num_sprites = obj_group.child("properties").child("property").last_attribute().as_int();

	anim->sprites = new Sprite[anim->num_sprites];
	int i = 0;

	for (pugi::xml_node object = obj_group.child("object"); object; object = object.next_sibling("object"))
	{
		anim->sprites[i].rect.x = object.attribute("x").as_int();
		anim->sprites[i].rect.y = object.attribute("y").as_int();
		anim->sprites[i].rect.w = object.attribute("width").as_int();
		anim->sprites[i].rect.h = object.attribute("height").as_int();

		anim->sprites[i++].frames = object.child("properties").child("property").attribute("value").as_int();
	}
}

void j1Player::LoadAABB(pugi::xml_node& player_node)
{
	if (strcmp(player_node.child("objectgroup").attribute("name").as_string(), "AABB") == 0) {
		pugi::xml_node obj_group = player_node.child("objectgroup");

		for (pugi::xml_node object = obj_group.child("object"); object; object = object.next_sibling("object"))
		{
			SDL_Rect rect;
			p2Point<int> offset{ 0,0 };
			offset = { object.attribute("x").as_int() ,object.attribute("y").as_int() };
			rect.x = player_pos.x + offset.x;
			while (offset.y - player_tmx_data.tile_height > 0 || offset.y - player_tmx_data.tile_height > player_tmx_data.tile_height) {
				offset.y -= player_tmx_data.tile_height;
			}
			rect.y = player_pos.y + offset.y;
			rect.w = object.attribute("width").as_int();
			rect.h = object.attribute("height").as_int();
		

			if (strcmp(object.attribute("name").as_string(), "AABB_IDLE") == 0) 
			{
				AABB_sheathed_idle = App->collider->AddCollider(rect, COLLIDER_PLAYER);
			}		
			if (strcmp(object.attribute("name").as_string(), "AABB_RUN") == 0) 
			{
				AABB_sheathed_run = App->collider->AddCollider(rect, COLLIDER_PLAYER);
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
			AABB_current = AABB_sheathed_run;
		}
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP) {
		if (currentAnimation != sheathed_idle) {
			currentAnimation->ResetAnim();
			currentAnimation = sheathed_idle;
			AABB_current = AABB_sheathed_idle;
		}
	}

	
	Draw(); //Draw all the player
	return true;
}



bool j1Player::PostUpdate()
{
	return true;
}



void j1Player::Draw()
{

	App->render->Blit(player_tmx_data.texture, player_pos.x, player_pos.y, &currentAnimation->sprites[currentAnimation->GetSprite()].rect,3.f);

	//Draw Collider
	SDL_Rect rect;
	float scale = App->win->GetScale();
	rect.x = AABB_current->rect.x * App->render->player_size * scale;
	rect.y = AABB_current->rect.y * App->render->player_size * scale;
	rect.w = AABB_current->rect.w * App->render->player_size * scale;
	rect.h = AABB_current->rect.h * App->render->player_size * scale;
	App->render->DrawQuad(rect, 0, 255, 0, 100);

}
