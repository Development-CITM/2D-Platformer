#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// ----------------------------------------------------
struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;
		
		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

struct MapLayer
{
	p2SString	name;
	uint		width = 0u;
	uint		height = 0u;
	int*		data = NULL;
	float		speed=0.0f;
	Properties	properties;

	MapLayer()  : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	// TODO 6 (old): Short function to get the value of x,y
	inline uint Get(int x, int y) const {
		return data[(y* width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	uint				firstgid = 0u;
	uint				margin = 0u;
	uint				spacing = 0u;
	uint				tile_width = 0u;
	uint				tile_height = 0u;
	SDL_Texture*		texture = nullptr;
	uint				tex_width = 0u;
	uint				tex_height = 0u;
	uint				num_tiles_width = 0u;
	uint				num_tiles_height = 0u;
	int					offset_x = 0;
	int					offset_y = 0;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

// ----------------------------------------------------
struct MapData
{
	uint				width = 0u;
	uint				height = 0u;
	uint				tile_width = 0u;
	uint				tile_height = 0u;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;


};

// ----------------------------------------------------
class j1Tilesets : public j1Module
{
public:

	j1Tilesets();

	// Destructor
	virtual ~j1Tilesets();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();

	bool Update(float dt) override;
	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	TileSet* GetTilesetFromTileId(int id) const;

	SDL_Rect	GetRect(TileSet*, int id);

	// Coordinate translation methods
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	//Map Loading Function
	bool		LoadMap(pugi::xml_node& map);

	//Tilesets loading functions
	bool		LoadAllTileset(pugi::xml_node& tileset);

	bool		LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);

	bool		LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);

	//Layers loading functions
	bool		LoadAllLayers(pugi::xml_node& folder_group);

	bool		LoadLayer(pugi::xml_node& node, MapLayer* layer);

	//Load Properties
	bool		LoadProperties(pugi::xml_node& node, Properties& properties);

	//Objects loading functions
	bool		LoadAllObjects(pugi::xml_node& object);

	bool		LoadObject(pugi::xml_node& node);

	//Log map info function
	void		LogLayerInfo(const char* file_name);
	
	//
	void		SetCullingPos(pugi::xml_node& object);
	void		SetCullingPosFromCurrentLevel(pugi::xml_node& it);
	

public:

	MapData					map_Data;

	Collider*				culling_Collider = nullptr;
	int						culling_pos_x = 0;
	int						culling_pos_y = 0;

	//PathFinding

	bool show_path = false;

private:

	p2SString				folder;
	bool					map_loaded;
	p2List_item<MapLayer*>* lay = nullptr;
	p2List_item<TileSet*>*	tile = nullptr;
	TileSet*				set = nullptr;
	MapLayer*				lays = nullptr;
	int						layercnt = 0;

	int						currentFrame=0;
	int						maxFrames=0;

	bool					coins_backup_once = true;

};

#endif // __j1MAP_H__