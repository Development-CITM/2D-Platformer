#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


struct Collider;
enum MapTypes;

// ----------------------------------------------------
struct Animation {
	int numRects = 0;
	int numFrame = 0;
	int repeatFrames = 0;

	p2SString name;

	SDL_Rect* rects = nullptr;

	bool loop = false;
	
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
	uint					width = 0u;
	uint					height = 0u;
	uint					tile_width = 0u;
	uint					tile_height = 0u;
	SpriteSheet				spriteSheet;
	MapTypes				type;
	p2List<ObjectLayer*>	object_Layers;
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

	//Called when collided
	void OnCollision(Collider*,Collider*);


	bool Load(const char* path);

	void SetAnimations(Animation**);

	bool LoadLayer(pugi::xml_node& node, ObjectLayer* layer);

	bool LoadSpriteSheet(pugi::xml_node& node);


private:

	bool LoadMap();
	
	void MoveToPosition(p2Point<int> targetPos);

	bool WantToMove(p2Point<int> targetPos);

public:
	PlayerTMXData		player_tmx_data;

	Collider*			player_Collider = nullptr;
	Collider*			ground_Collider = nullptr;

	bool				GetDetectedCollision() { return detected_Collision; }
	bool				SetDetectedCollision(bool set) { return detected_Collision = set; }
private:

	p2Point<int>		playerPos = { 0,0 };
	p2Point<int>		previousPos = { 0,0 };

	p2Point<int>		forwardVector = { 2,0 };	//Need to change to variable speed (do a operator overload to multiply or sum that variable)
	p2Point<int>		backwardVector = { -2,0 };	//Need to change to variable speed (do a operator overload to multiply or sum that variable)

	PlayerState			state = ST_IDLE;

	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	Animation*			currentAnimation = nullptr;
	Animation*			previousAnimation = nullptr;

	Animation*			idle = nullptr;
	Animation*			run = nullptr;

	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded = false;

	bool				detected_Collision = false;

	bool				onGround = false;

};

#endif // __j1PLAYER_H__