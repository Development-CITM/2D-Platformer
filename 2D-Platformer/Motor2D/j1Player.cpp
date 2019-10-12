#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include <math.h>


j1Player::j1Player()
{
	name.create("player");
}

bool j1Player::Awake(pugi::xml_node& conf)
{
	LOG("Loading Player");
	bool ret = true;

	return ret;
}

bool j1Player::Start()
{
	Load("animations/Player.tmx");
	return true;
}

bool j1Player::PreUpdate()
{
	return true;
}

bool j1Player::Update(float dt)
{
	Draw();
	return true;
}

bool j1Player::PostUpdate()
{
	return true;
}

void j1Player::Draw()
{
	int num = 3;
	if (animations[num]->repeatFrames > 6) {
		if (animations[num]->numFrame < animations[num]->numRects - 1)
		{
			//App->render->Blit(player_tmx_data.spriteSheet.texture, 200, 472, &idle->rects[idle->numFrame],App->render->drawsize);
			animations[num]->numFrame++;
		}
		else {
			animations[num]->numFrame = 0;
		}
		animations[num]->repeatFrames = 0;
		
	}
	else {
		animations[num]->repeatFrames++;
	}
	App->render->Blit(player_tmx_data.spriteSheet.texture, 200, 472, &animations[num]->rects[animations[num]->numFrame],App->render->drawsize);
	
	
}

bool j1Player::CleanUp()
{
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
		ObjectLayer* lay = new ObjectLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			player_tmx_data.layers.add(lay);
	}
	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", player_tmx_data.width, player_tmx_data.height);
		LOG("tile_width: %d tile_height: %d", player_tmx_data.tile_width, player_tmx_data.tile_height);


		p2List_item<ObjectLayer*>* item_layer = player_tmx_data.layers.start;
		animations = new Animation*[player_tmx_data.layers.count()];
		int i = 0;
		while (item_layer != NULL)
		{
			animations[i] = new Animation;
			ObjectLayer* l = item_layer->data;
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
	LoadAnimations();
	map_loaded = ret;

	return ret;
}

void j1Player::LoadAnimations() {
	idle = animations[0]; 
	LOG("Animation name: %s", idle->name.GetString());
	for (int i = 0; i < player_tmx_data.layers.count(); i++)
	{	
		SDL_Rect* r = animations[i]->rects;
		LOG("ADDING ANIMATIONS---");
		LOG("Animation name: %s", animations[i]->name.GetString());		
		LOG("Num rects: %d", animations[i]->numRects);
		for (int j = 0; j < animations[i]->numRects; j++)
		{
		LOG("x= %d y= %d width= %d height= %d", animations[i]->rects[j].x, animations[i]->rects[j].y, animations[i]->rects[j].w, animations[i]->rects[j].h);

		}
		
	}
}

// Load map general properties
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
