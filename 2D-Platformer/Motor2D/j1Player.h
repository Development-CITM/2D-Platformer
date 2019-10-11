#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


enum MapTypes;
// ----------------------------------------------------
struct ObjectLayer
{
	p2SString	name;
	p2List<SDL_Rect*> rects;
};
struct PlayerTMXData {
	uint					width;
	uint					height;
	uint					tile_width;
	uint					tile_height;
	SDL_Texture*			texture;
	MapTypes				type;
	p2List<ObjectLayer*>	layers;
};


class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player() {};

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Load(const char* path);

	bool LoadLayer(pugi::xml_node& node, ObjectLayer* layer);


private:

	bool LoadMap();
	//bool LoadTilesetDetails(pugi::xml_node& tileset_node);
	//bool LoadTilesetImage(pugi::xml_node& tileset_node);
	//bool LoadLayer(pugi::xml_node& node);
public:
	PlayerTMXData player_tmx_data;

private:

	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded;
	int					currentFrame;
	int					maxFrames;
};

#endif // __j1PLAYER_H__