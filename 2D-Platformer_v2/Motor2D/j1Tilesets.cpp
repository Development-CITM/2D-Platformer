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

// Awake -----------------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::Awake(pugi::xml_node& config)
{
	bool ret = true;

	LOG("Loading Map Parser");
	folder.create(config.child("folder").child_value());
	
	
	return ret;
}

// Start -----------------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::Start()
{
	return true;
}

// Update ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::Update(float dt)
{
	return true;
}

// CleanUp ---------------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::CleanUp()
{
	LOG("Unloading map");
	// Remove all tilesets --------------------------------------------------------------------------------------------------
	p2List_item<TileSet*>* item;
	item = map_Data.tilesets.start;

	while (item != NULL)
	{
		SDL_DestroyTexture(item->data->texture);
		RELEASE(item->data);
		item = item->next;
	}
	map_Data.tilesets.clear();

	// Remove all layers ----------------------------------------------------------------------------------------------------
	p2List_item<MapLayer*>* item2;
	item2 = map_Data.layers.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	map_Data.layers.clear();
	return true;
}
#pragma endregion




#pragma region Render
//Draw all layers from each tileset used using GIDs ----------------------------------------------------------------------------------------------------------------
void j1Tilesets::Draw()
{
	if (map_loaded == false)
		return;

	//Init to iterate the layers list ----------------------------------------------------------------------------------------
	p2List_item<MapLayer*>* lay = this->map_Data.layers.start;
	MapLayer* layer = lay->data;
	for (int i = 0; i < map_Data.layers.count(); i++)
	{
		for (int y = 0; y < map_Data.height; ++y)
		{
			for (int x = 0; x < map_Data.width; ++x)
			{
				//Gets the ID of the respective tile from the coords in Tiled -----------------------------------------------
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					//Gets the respective tile from the tileset which is wanted to drawn ------------------------------------
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						//Does the conversion from Tiled map to an Orthogonal world -----------------------------------------
						iPoint pos = MapToWorld(x, y);

						// Parallax -----------------------------------------------------------------------------------------
						if (strcmp(layer->name.GetString(), "Mountain") == 0 || strcmp(layer->name.GetString(), "Rocks 2")==0 || strcmp(layer->name.GetString(), "Mountain 2")==0 || strcmp(layer->name.GetString(), "Rocks") == 0)
						{
							if (culling_Collider->CheckCollision({ (int)(pos.x + (App->render->camera.x  * layer->speed)),pos.y,r.w,r.h })) {
								pos.x = pos.x + (App->render->camera.x * layer->speed);
								App->render->Blit(tileset->texture, pos.x, pos.y, &r, App->render->drawsize);
							}
						}
						else if (strcmp(layer->name.GetString(), "Sea") == 0)
						{
							if (culling_Collider->CheckCollision({  pos.x,pos.y ,r.w,r.h }))
								App->render->Blit(tileset->texture, pos.x, pos.y, &r, App->render->drawsize);
						}
						// Default draw -------------------------------------------------------------------------------------
						else
						{
							if (culling_Collider->CheckCollision({ pos.x,pos.y,r.w,r.h }))
						App->render->Blit(tileset->texture, pos.x, pos.y, &r, 2.0f);
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
// Load new Map -----------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::Load(const char* file_name)
{
	//Gets source of the map passed from Scene
	bool ret = true;
	pugi::xml_document		map_file;
	p2SString tmp("%s%s", folder.GetString(), file_name); 
	pugi::xml_parse_result result = map_file.load_file(file_name);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	pugi::xml_node tileset = map_file.child("map").child("tileset");
	pugi::xml_node object = map_file.child("map").child("objectgroup");
	pugi::xml_node folder_group = map_file.child("map").child("group").child("group");
	pugi::xml_node map = map_file.child("map");
	
	// Load general info -------------------------------------------------------------------------------------------------------
	if(ret)
		ret = LoadMap(map);
	
	// Load all tilesets info --------------------------------------------------------------------------------------------------
	if(ret)
		ret= LoadAllTileset(tileset);
	
	// Load all layer info from the map -------------------------------------------------------------------------------------------
	if(ret)
		ret = LoadAllLayers(folder_group);
		
	//Load objects info from the map ------------------------------------------------------------------------------------------
	if(ret)
		ret = LoadAllObjects(object);
	

	//Logs all the info loaded on top of this ---------------------------------------------------------------------------------
	if(ret)
		LogLayerInfo(file_name);
	
	map_loaded = ret;
	return ret;
}

// Loads all the layers from it's folders in Tiled ------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadAllLayers(pugi::xml_node &folder_group)
{
	bool ret = true;
	//(group->group(iterate)->layer(iterate)) ----------------------------------------------------------------------------------
	for (folder_group; folder_group; folder_group =folder_group.next_sibling("group"))
	{
		for (pugi::xml_node layer = folder_group.child("layer"); layer; layer=layer.next_sibling("layer"))
		{
			MapLayer* lay = new MapLayer();

			ret = LoadLayer(layer, lay);

			if (ret == true)
				map_Data.layers.add(lay);
		}
	}
	return ret;
}

// Loads map propperties: Map orientation widths,hegihts ------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadMap(pugi::xml_node &map)
{
	bool ret = true;
	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		//Dimension attributes -------------------------------------------------------------------------------------------------
		map_Data.width = map.attribute("width").as_int();
		map_Data.height = map.attribute("height").as_int();
		map_Data.tile_width = map.attribute("tilewidth").as_int();
		map_Data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		//Color set ------------------------------------------------------------------------------------------------------------
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

		// Orientation of the map ---------------------------------------------------------------------------------------------
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

//Loads all tilesets ------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadAllTileset(pugi::xml_node& tileset)
{
	bool ret = true;
	for (tileset; tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		set = new TileSet();
		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}
		map_Data.tilesets.add(set);

	}
	return ret;
}

//Loads tilesets's data used in lvl building ------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	//Tileset attributes -----------------------------------------------------------------------------------------------------
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node properties = tileset_node.child("properties").child("property");

	//Any Offsets ------------------------------------------------------------------------------------------------------------
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

//Loads tilesets's pngs from a source provided on LoadTilesetDetails() ----------------------------------------------------------------------------------------------
bool j1Tilesets::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

if (image == NULL)
{
	LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
	ret = false;
}
//Creation of texture from tilesets's details ---------------------------------------------------------------------------------
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

//Loads layer attributes provided from SendNode() -------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;
	//Loading all layer attributes  -----------------------------------------------------------------------------------------
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
	//Loading each tile from the layer into an array (The respective GID from the tileset) ----------------------------------
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

//Logs all layers data loaded ---------------------------------------------------------------------------------------------------------------------------------------
void j1Tilesets::LogLayerInfo(const char* file_name)
{
	LOG("Successfully parsed map XML file: %s", file_name);
	LOG("width: %d height: %d", map_Data.width, map_Data.height);
	LOG("tile_width: %d tile_height: %d", map_Data.tile_width, map_Data.tile_height);

	//Logging tilesets used ------------------------------------------------------------------------------------------------
	p2List_item<TileSet*>* item = map_Data.tilesets.start;
	while (item != NULL)
	{
		TileSet* s = item->data;
		LOG("Tileset ----");
		LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
		LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
		LOG("spacing: %d margin: %d", s->spacing, s->margin);
		item = item->next;
	}
	//Logging layers info --------------------------------------------------------------------------------------------------
	p2List_item<MapLayer*>* item_layer = map_Data.layers.start;
	while (item_layer != NULL)
	{
		MapLayer* l = item_layer->data;
		LOG("Layer ----");
		LOG("name: %s", l->name.GetString());
		LOG("tile width: %d tile height: %d", l->num_tile_width, l->num_tile_height);
		item_layer = item_layer->next;
	}
}

//Loads all objects -------------------------------------------------------------------------------------------------------------------------------------------------
bool j1Tilesets::LoadAllObjects(pugi::xml_node& object)
{
	bool ret = true;
	for (object; object; object = object.next_sibling("objectgroup"))
	{
		LoadObject(object);
	}
	if (App->scene->swapping)
	{
		App->scene->swapping = false;
	}
	return ret;
}

//Loads each object layer of the map with it's respective function in another modules -------------------------------------------------------------------------------
bool j1Tilesets::LoadObject(pugi::xml_node& node)
{
	bool ret = true;
	pugi::xml_node objects = node;
	//Colliders map loading ------------------------------------------------------------------------------------------------
	if (strcmp(node.attribute("name").as_string(), "Colliders") == 0)
	{
		objects = node.child("object");
		App->collider->Load(objects);
	}
	//Music map loading ----------------------------------------------------------------------------------------------------
	else if (strcmp(node.attribute("name").as_string(), "Music") == 0)
	{
		objects = node.child("object").child("properties").child("property");
		App->audio->PlayMusic(objects.attribute("value").as_string());
	}
	//Camera limit map loading ---------------------------------------------------------------------------------------------
	else if (strcmp(node.attribute("name").as_string(), "Camera Limit") == 0 )
	{
		objects = node.child("object");
		App->scene->LoadSceneLimits(objects);
	}
	//Culling pos map loading ----------------------------------------------------------------------------------------------
	else if (strcmp(node.attribute("name").as_string(), "Culling_pos") == 0)
	{
		objects = node.child("object");
		SetCullingPos(objects);
	}
	else if (strcmp(node.attribute("name").as_string(), "Camera_pos") == 0)
	{
		objects = node.child("object");
		App->render->SetCameraPos(objects);
	}
	else if (strcmp(node.attribute("name").as_string(), "Player_pos") == 0)
	{
		objects = node.child("object");
		App->player->SetPlayerPos(objects);
	}

	return ret;
}

//Sets the position of culling collider from Tiled map attributes ---------------------------------------------------------------------------------------------------
void j1Tilesets::SetCullingPos(pugi::xml_node& object)
{
	//Sets Culling Pos ----------------------------------------------------------------------------------------------------
	if( App->scene->loading == false)
	{
		if (App->scene->swapping == true)
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) //EUDALD: Check this when changing start cameras and cullings
			{
				SetCullingPosFromCurrentLevel(it);
			}
		}
		else
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) //EUDALD: Check this when changing start cameras and cullings
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
	}
	//Sets Culling Collider ------------------------------------------------------------------------------------------------
	p2Point<int> culling_Pos{ culling_pos_x,culling_pos_y };
	if (App->scene->loading == false)
	{
		
		culling_Collider = App->collider->AddCollider({ culling_Pos.x,culling_Pos.y,App->win->GetWidth() / 2,App->win->GetHeight() / 2 }, COLLIDER_WINDOW);
	}
	else
	{
		culling_Collider = App->collider->AddCollider({ culling_Pos.x,culling_Pos.y,App->win->GetWidth() / 2,App->win->GetHeight() / 2 }, COLLIDER_WINDOW);
	}
}
void j1Tilesets::SetCullingPosFromCurrentLevel(pugi::xml_node& it)
{
	char* pos_x = "culling_pos_x";
	char* pos_y = "culling_pos_y";
	if (App->scene->current_level == App->scene->A1)
	{
		pos_x = "culling_pos_x_swap_from_A1";
		pos_y = "culling_pos_y_swap_from_A1";
	}
	else if (App->scene->current_level == App->scene->A2)
	{
		pos_x = "culling_pos_x_swap_from_A2";
		pos_y = "culling_pos_y_swap_from_A2";
	}
	else if (App->scene->current_level == App->scene->A3)
	{
		pos_x = "culling_pos_x_swap_from_A3";
		pos_y = "culling_pos_y_swap_from_A3";
	}
	else if (App->scene->current_level == App->scene->A5)
	{
		pos_x = "culling_pos_x_swap_from_A5";
		pos_y = "culling_pos_y_swap_from_A5";
	}

	if (strcmp(it.attribute("name").as_string(), pos_x) == 0)
	{
		culling_pos_x = it.attribute("value").as_int();
	}
	if (strcmp(it.attribute("name").as_string(), pos_y) == 0)
	{
		culling_pos_y = it.attribute("value").as_int();
	}
}
#pragma endregion





#pragma region Logic Operations
//Gets the respective tileset needed for printing the needed GID ----------------------------------------------------------------------------------------------------
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

//Does the Logical Operations to transform from Tiled to World -------------------------------------------------------------------------------------------------------
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

//Does the Logical Operations to transform from World to Tiled -------------------------------------------------------------------------------------------------------
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

//Gets the size and position of the Tiled needed to be printed -------------------------------------------------------------------------------------------------------
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

//Gets the size and position of the Rect which is gonna be printed ---------------------------------------------------------------------------------------------------
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


