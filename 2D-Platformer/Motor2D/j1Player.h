#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "Animation.h"

//Forward Declarations
struct Collider;
enum MapTypes;

#pragma region Enums

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
enum Directions
{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN,
	DIR_PLATFORM,
	DIR_NONE,
};
#pragma endregion

#pragma region Structs

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
#pragma endregion


class j1Player : public j1Module
{
//------------FUNCTIONS-----------//
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

	// Called each loop iteration if has one
	bool Load(const char* path);


	// Called each loop iteration if has one
	void Draw();

	// Called before quitting
	bool CleanUp();

	//Called when collided
	//void OnCollision(Collider*, Collider*);
	void SetPlayerPos(pugi::xml_node& node);

	Collider* GetPlayerCollider() const { return player_Collider; }
	int GetVelocityX() { return velocity_X; }

private:

	

	//Inputs
	void HorizontalInput();
	void VerticalInput();

	void DashInput();

	void ExitInput();

	//Habilities
	void Dash();

	//Logic Movements
	void Move();
	void Jump();
	void Gravity();
	void MoveOnGodMode();

	bool MoveTo(Directions dir);


	//Load Functions
	bool LoadMap();
	void SetAnimations(Animation**);
	bool LoadLayer(pugi::xml_node& node, ObjectLayer* layer);
	bool LoadSpriteSheet(pugi::xml_node& node);

	//Animation Functions
	void ChangeAnimation(Animation*);


//---------------VARIABLES --------------------//
public:
	PlayerTMXData		player_tmx_data;
	int					pivot_x_flip;
	int					player_pos_x;
	int					player_pos_y;

	p2Point<int>		playerPos = { 0,0 };

	//Colliders
	Collider* player_Collider = nullptr;

	bool				god_mode_enabled = false;
	bool				onPlatform = false;
	bool				jumping = false;

	PlayerState			state = ST_IDLE;
	bool				canMove = true;

	void				ResetInputs();

private:

	//Enums

	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	Directions			direction = DIR_RIGHT;
	Directions			last_Direction = DIR_RIGHT;

	//Positions

	p2Point<int>		previousColliderPos = { 0,0 };
	p2Point<int>		previousPlayerPos{ 0,0 };
	p2Point<int>		currentVelocity{ 0,0 };

	//Actual movement speed  direction
	int					velocity_X = 0;
	int					velocity_Y = 0;

	//Jump Variables
	uint				maxJump = 0u;
	uint				jumpDistance;
	uint				timeOnAir;
	uint				currentTimeAir = 0u;

	//Speeds
	uint				jumpSpeed = 0u;
	uint				max_jumpSpeed;
	uint				fallSpeed = 0u;
	uint				max_FallSpeed;
	uint				gravityForce;
	uint				max_gravityForce;
	uint				runSpeed;

	//Heights for adjustment while DIR_DOWN
	int playerheight_dir_down;
	int colliderheight_dir_down;

	//Animations
	Animation*			currentAnimation = nullptr;
	Animation*			previousAnimation = nullptr;

	Animation*			idle = nullptr;
	Animation*			run = nullptr;
	Animation*			jump = nullptr;
	Animation*			fall = nullptr;


	//Checkers
	bool				detected_Collision = false;
	bool				onGround = false;


	bool				move_To_Right = false;
	bool				move_To_Left = false;
	bool				move_To_Up = false;


	bool				dash = false;

	//Rects
	SDL_Rect* rect;

	//Object Layer
	ObjectLayer* lay;
	


	//XML Stuff
	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded = false;


	p2Point<int> relativePos = { 0,0 };
};


#endif // __j1PLAYER_H__