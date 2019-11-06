#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Audio.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Fade2Black.h"
#include <math.h>

#pragma region Constructor / Destructor

j1Tilesets::j1Tilesets() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Tilesets::~j1Tilesets()
{}
#pragma endregion

#pragma region Awake/CleanUp

// Called before render is available
bool j1Tilesets::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading Map Parser");
	folder.create(config.child("folder").child_value());
	
	
	return ret;
}

bool j1Tilesets::Start()
{
	return true;
}

bool j1Tilesets::Update(float dt)
{

	return true;
}

// Called before quitting
bool j1Tilesets::CleanUp()
{
	LOG("Unloading map");
	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = map_Data.tilesets.start;

	

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	map_Data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = map_Data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	map_Data.layers.clear();

	// Clean up the pugi tree
	map_file.reset();

	return true;
}

#pragma endregion

#pragma region Render

void j1Tilesets::Draw()
{

	if (map_loaded == false)
		return;

	//----------------------------------------------------------------------------------//

	p2List_item<MapLayer*>* lay = this->map_Data.layers.start;
	MapLayer* layer = lay->data;
	for (int i = 0; i < map_Data.layers.count(); i++)
	{
		for (int y = 0; y < map_Data.height; ++y)
		{
			for (int x = 0; x < map_Data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{

					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);
						if (strcmp(layer->name.GetString(), "Clouds") == 0)
						{
							if (culling_Collider->CheckCollision({ (int)(pos.x + (App->render->camera.x  * layer->speed)),pos.y,r.w,r.h })) {
								pos.x = pos.x + (App->render->camera.x * layer->speed);
								App->render->Blit(tileset->texture, pos.x, pos.y, &r, App->render->drawsize,true);
							}
						}
						else if (strcmp(layer->name.GetString(), "Sea") == 0)
						{
							if (culling_Collider->CheckCollision({  pos.x,pos.y ,r.w,r.h }))
						App->render->Blit(tileset->texture, pos.x, pos.y, &r, App->render->drawsize);
						}
						else
						{
							if (culling_Collider->CheckCollision({ pos.x,pos.y,r.w,r.h }))
						App->render->Blit(tileset->texture, pos.x, pos.y, &r, App->render->drawsize);
						}
						
					}
				}
			}
		}
		if (lay->next != nullptr) {
			lay = lay->next;
			layer = lay->data;
		}
	}
}
#pragma endregion


#pragma region Load Info

// Load new map
bool j1Tilesets::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name); 

	pugi::xml_parse_result result = map_file.load_file(file_name);
	

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	for(pugi::xml_node tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}
		map_Data.tilesets.add(set);
	}
	

	// Load layer info ----------------------------------------------
	for(pugi::xml_node layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			map_Data.layers.add(lay);
	}

	//Load objects info -----------------------------------------------
	for (pugi::xml_node object = map_file.child("map").child("objectgroup"); object; object=object.next_sibling("objectgroup"))
	{
		LoadObject(object);
	}




	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", map_Data.width, map_Data.height);
		LOG("tile_width: %d tile_height: %d", map_Data.tile_width, map_Data.tile_height);

		p2List_item<TileSet*>* item = map_Data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = map_Data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->num_tile_width, l->num_tile_height);
			item_layer = item_layer->next;
		}
	}
	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Tilesets::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		map_Data.width = map.attribute("width").as_int();
		map_Data.height = map.attribute("height").as_int();
		map_Data.tile_width = map.attribute("tilewidth").as_int();
		map_Data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		map_Data.background_color.r = 0;
		map_Data.background_color.g = 0;
		map_Data.background_color.b = 0;
		map_Data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) map_Data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) map_Data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) map_Data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			map_Data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			map_Data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			map_Data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			map_Data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Tilesets::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node properties = tileset_node.child("properties").child("property");

	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Tilesets::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

if (image == NULL)
{
	LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
	ret = false;
}
else
{
	set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
	int w, h;
	SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
	set->tex_width = image.attribute("width").as_int();

	if (set->tex_width <= 0)
	{
		set->tex_width = w;
	}

	set->tex_height = image.attribute("height").as_int();

	if (set->tex_height <= 0)
	{
		set->tex_height = h;
	}

	set->num_tiles_width = set->tex_width / set->tile_width;
	set->num_tiles_height = set->tex_height / set->tile_height;
}

return ret;
}

bool j1Tilesets::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->num_tile_width = node.attribute("width").as_int();
	layer->num_tile_height = node.attribute("height").as_int();
	if (node.child("properties").child("property").attribute("name"))
	{
		layer->speed = node.child("properties").child("property").attribute("value").as_float();
	}
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new int[layer->num_tile_width * layer->num_tile_height];
		memset(layer->data, 0, layer->num_tile_width * layer->num_tile_height);

		int i = 0;
		for (pugi::xml_node tiles = layer_data.child("tile"); tiles; tiles = tiles.next_sibling("tile"))
		{
			layer->data[i++] = (int)tiles.attribute("gid").as_int(0);
		}
	}

	return ret;
}
bool j1Tilesets::LoadObject(pugi::xml_node& node)
{
	bool ret = true;
	pugi::xml_node objects = node;
	if (strcmp(node.attribute("name").as_string(), "Colliders") == 0)
	{
		objects = node.child("object");
		App->collider->Load(objects);
	}
	else if (strcmp(node.attribute("name").as_string(), "Music") == 0)
	{
		objects = node.child("object").child("properties").child("property");
		App->audio->PlayMusic(objects.attribute("value").as_string());
	}
	else if (strcmp(node.attribute("name").as_string(), "Camera Limit") == 0 )
	{
		objects = node.child("object");
		App->scene->LoadSceneLimits(objects);
	}

	else if (strcmp(node.attribute("name").as_string(), "Culling_pos") == 0)
	{
		objects = node.child("object");
		SetCullingPos(objects);
	}

	return ret;
}
void j1Tilesets::SetCullingPos(pugi::xml_node& object)
{
	if( App->scene->loading == false)
	{
		for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property"))
		{
			if (strcmp(it.attribute("name").as_string(), "culling_pos_x") == 0)
			{
				culling_pos_x = it.attribute("value").as_int();
			}
			if (strcmp(it.attribute("name").as_string(), "culling_pos_y") == 0)
			{
				culling_pos_y = it.attribute("value").as_int();
			}
		}
	}
	p2Point<int> culling_Pos{ culling_pos_x,culling_pos_y };
	if (App->scene->loading == false)
	{
		culling_Collider = App->collider->AddCollider({ culling_Pos.x,culling_Pos.y,App->win->GetWidth() / 2,App->win->GetHeight() / 2 }, COLLIDER_WINDOW);
	}
	else
	{
		culling_Collider = App->collider->AddCollider({ culling_Collider->rect.x,culling_Collider->rect.y,App->win->GetWidth() / 2,App->win->GetHeight() / 2 }, COLLIDER_WINDOW);
	}
}
#pragma endregion

#pragma region Logic Operations
TileSet* j1Tilesets::GetTilesetFromTileId(int id) const
{
	bool ret = false;
	p2List_item<TileSet*>* tile = nullptr;
	tile = map_Data.tilesets.start;
	while (ret == false) {
		if (tile->next != NULL && tile->next->data->firstgid <= id) {
			tile = tile->next;
		}
		else if (tile->prev != NULL && tile->data->firstgid > id) {
			tile = tile->prev;
		}
		else ret = true;
	}

	return tile->data;
}
iPoint j1Tilesets::MapToWorld(int x, int y) const
{
	iPoint ret(0, 0);

	switch (map_Data.type)
	{
	case MapTypes::MAPTYPE_ORTHOGONAL:
		ret.x = x * map_Data.tile_width;
		ret.y = y * map_Data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		ret.x = (x - y) * map_Data.tile_width / 2;
		ret.y = (x + y) * map_Data.tile_height / 2;

		break;
	}
	return ret;
}


iPoint j1Tilesets::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	switch (map_Data.type)
	{
	case MapTypes::MAPTYPE_ORTHOGONAL:
		ret.x = x / map_Data.tile_width;
		ret.y = y / map_Data.tile_height;
		break;
	case MapTypes::MAPTYPE_ISOMETRIC:
		ret.x = (x / map_Data.tile_width / 2 + y / map_Data.tile_height / 2) / 2;
		ret.y = (y / map_Data.tile_height / 2 - (x / map_Data.tile_width / 2)) / 2;
	default:
		break;
	}
	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}


SDL_Rect j1Tilesets::GetRect(TileSet* tileset, int id)
{
	int num = id;

	int x = (num - tileset->firstgid) % tileset->num_tiles_width;
	int y = (num - tileset->firstgid) / tileset->num_tiles_width;
	int width = 0;
	int height = 0;

	width = x * map_Data.tile_width + (x + tileset->margin) * tileset->margin;
	height = y * map_Data.tile_width + (y + tileset->spacing) * tileset->spacing;
	

	SDL_Rect rect = { width,height,tileset->tile_width,tileset->tile_height };
	return	rect;
}
#pragma endregion


