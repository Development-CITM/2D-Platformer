#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// ----------------------------------------------------


class j1Player : public j1Module
{
public:

	j1Player();

	// Destructor
	virtual ~j1Player();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);


private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node);
	bool LoadTilesetImage(pugi::xml_node& tileset_node);
	bool LoadLayer(pugi::xml_node& node);



private:

	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
	int					currentFrame;
	int					maxFrames;
};

#endif // __j1PLAYER_H__