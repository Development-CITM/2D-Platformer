#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	uint		num_tile_width = 0u;
	uint		num_tile_height = 0u;
	int*		data = NULL;

	MapLayer()  : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	// TODO 6 (old): Short function to get the value of x,y
	inline uint Get(int x, int y) const {
		return data[(y* num_tile_width) + x];
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
	SDL_Texture*		texture;
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

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	TileSet* GetTilesetFromTileId(int id) const;

	// Coordinate translation methods
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

private:

	bool		LoadMap();
	bool		LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool		LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool		LoadLayer(pugi::xml_node& node, MapLayer* layer);
	SDL_Rect	GetRect(TileSet* ,int id);
	bool		LoadObject(pugi::xml_node& node);
	void		SetCullingPos(pugi::xml_node& object);
	

public:

	MapData		map_Data;

	Collider*			culling_Collider = nullptr;
	Collider*			camera_Collider = nullptr;
private:

	pugi::xml_document	map_file;

	p2SString				folder;
	bool					map_loaded;
	p2List_item<MapLayer*>* lay = nullptr;
	p2List_item<TileSet*>*	tile = nullptr;
	TileSet* set;
	MapLayer* lays;
	int layercnt = 0;


	//pugis
	pugi::xml_parse_result result;
	pugi::xml_node tileset;
	pugi::xml_node layer;
	pugi::xml_node object;
	pugi::xml_node map;
	pugi::xml_node properties;
	pugi::xml_node offset;
	pugi::xml_node image;
	pugi::xml_node layer_data;
	pugi::xml_node tiles;
	pugi::xml_node objects;
	pugi::xml_node it;

	int						currentFrame;
	int						maxFrames;
	int						culling_pos_x = 0;
	int						culling_pos_y = 0;

};

#endif // __j1MAP_H__