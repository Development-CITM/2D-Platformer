#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


enum MapTypes;
// ----------------------------------------------------
struct Animation {
	int numRects = 0;
	p2SString name;
	SDL_Rect* rects;
	int numFrame = 0;
	bool loop = false;
	int repeatFrames = 0;
	
	void ResetAnim() {
		numFrame = 0;
	}
};

enum PlayerState {
	ST_IDLE,
	ST_RUNNING,
};

struct ObjectLayer
{
	p2SString			name;
	p2List<SDL_Rect*>	rects;
};

struct SpriteSheet {
	SDL_Texture*	texture = nullptr;
	uint			width = 0u;
	uint			height = 0u;
};
struct PlayerTMXData {
	uint					width;
	uint					height;
	uint					tile_width;
	uint					tile_height;
	SpriteSheet				spriteSheet;
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

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	bool Load(const char* path);

	void LoadAnimations();

	bool LoadLayer(pugi::xml_node& node, ObjectLayer* layer);

	bool LoadSpriteSheet(pugi::xml_node& node);


private:

	bool LoadMap();
	
	void MoveToPosition(p2Point<int> targetPos);

public:
	PlayerTMXData player_tmx_data;

private:

	p2Point<int>		playerPos = { 0,0 };
	p2Point<int>		forwardVector = { 2,0 };
	p2Point<int>		backwardVector = { -2,0 };

	PlayerState			state;


	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	Animation**			animations;
	Animation*			currentAnimation;
	Animation*			previousAnimation;
	Animation*			idle;
	Animation*			run;
	p2List<SDL_Rect*>	rect;
	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded;

};

#endif // __j1PLAYER_H__