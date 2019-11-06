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

	LOG("Loading Player");

	return ret;	
}

bool j1Player::Start()
{
	//Load Player tmx (it contains animations and colliders properties)
	Load("animations/Player.tmx");

	return true;
}

bool j1Player::CleanUp()
{
	// Remove all layers
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


	// Clean up the pugui tree
	player_file.reset();

	return true;
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

		lay = new ObjectLayer(); //Create new ObjectLayer to create new adress of ObjectLayer type

		ret = LoadLayer(layer, lay); //Layer is a node to layer node, and Lay its the adress of the new ObjectLayer to fill it





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


		}
		map_loaded = ret;

		return ret;
	}

	//void j1Player::SetAnimations(Animation** animations) {
	//
	//	for (int i = 0; i < player_tmx_data.object_Layers.count(); i++)
	//	{
	//		SDL_Rect* r = animations[i]->rects;
	//		p2SString name = animations[i]->name.GetString();
	//
	//		if (strcmp(name.GetString(), "Idle") == 0) {
	//
	//			idle = animations[i];
	//		}
	//		else if (strcmp(name.GetString(), "Running") == 0) {
	//			run = animations[i];
	//		}
	//		else if (strcmp(name.GetString(), "Jump") == 0) {
	//			jump = animations[i];
	//		}
	//		else if (strcmp(name.GetString(), "Falling") == 0) {
	//			fall = animations[i];
	//		}
	//
	//
	//	}
	//
	//	RELEASE_ARRAY(animations);
	//	currentAnimation = idle;
	//}

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


	return ret;
}

bool j1Player::LoadSpriteSheet(pugi::xml_node& node)
{
	bool ret = true;
	return ret;
}


bool j1Player::PreUpdate()
{

	return true;
}

bool j1Player::Update(float dt)
{	

	Draw(); //Draw all the player

	return true;
}



bool j1Player::PostUpdate()
{
	return true;
}



void j1Player::Draw()
{

	
}
