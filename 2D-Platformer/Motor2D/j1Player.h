#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"


struct Collider;
enum MapTypes;
enum Directions
{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN,
};
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
	ST_JUMP,
	ST_MIDAIR,
	ST_FALL,
	ST_JUMP_FINISHED,
};

enum Input {
	IN_IDLE,
	IN_RUNNING,
	IN_JUMP,
	IN_JUMP_FINISHED,
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

	void Jump();

	void ChangeAnimation(Animation*);

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

	//Movement Functions
	void HoldHorizontalMove();

	bool LoadMap();
	
	void MoveToPosition();

	bool WantToMove(Directions dir);

public:
	PlayerTMXData		player_tmx_data;

	Collider*			player_Collider = nullptr;
	Collider*			ground_Collider = nullptr;

	bool				GetDetectedCollision() { return detected_Collision; }
	bool				SetDetectedCollision(bool set) { return detected_Collision = set; }
private:

	p2Point<int>		playerPos = { 0,0 };
	p2Point<int>		lastPos = { 0,0 };
	p2Point<int>		nextPos = { 0,0 };
	p2Point<int>		previousPos = { 0,0 };

	p2Point<int>		forwardVector = { 2,0 };	//Need to change to variable speed (do a operator overload to multiply or sum that variable)
	p2Point<int>		backwardVector = { -2,0 };	//Need to change to variable speed (do a operator overload to multiply or sum that variable)
	p2Point<int>		upVector = { 0,-2 };	//Need to change to variable speed (do a operator overload to multiply or sum that variable)

	PlayerState			state = ST_IDLE;
	Input				last_input = IN_IDLE;

	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	Animation*			currentAnimation = nullptr;
	Animation*			previousAnimation = nullptr;

	Animation*			idle = nullptr;
	Animation*			run = nullptr;
	Animation*			jump = nullptr;

	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded = false;

	bool				detected_Collision = false;

	bool				onGround = false;

	p2Point<int>		maxVerticalJump{ 0,0 };
	int					jumpDistance = -40;

	bool				jumping = false;

	p2Point<int>		currentVelocity{ 0,0 };

	Directions			direction = DIR_RIGHT;

};

#endif // __j1PLAYER_H__