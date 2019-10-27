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

#define PLAYERMAXJUMP_GRAVITY4 40
#define PLAYERMAXJUMP_GRAVITY6 15
#define PLAYERMAXJUMP_GRAVITY9 5


#pragma region Enums

enum PlayerState {
	ST_IDLE,
	ST_RUNNING,
	ST_JUMP,
	ST_MIDAIR,
	ST_FALL,
	ST_JUMP_FINISHED,
	ST_DASH,
};

enum Directions
{
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN,
	DIR_PLATFORM,
	DIR_DASH_LEFT,
	DIR_DASH_RIGHT,
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
	int					pivot_x_flip=0;
	int					player_pos_x=0;
	int					player_pos_y=0;

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
	uint				jumpDistance = 0u;
	uint				timeOnAir = 0u;
	uint				currentTimeAir = 0u;

	//Dash Variables

	bool				dash = false;
	int					max_Dash = 0;
	uint				dash_distance = 0u;
	uint				dashSpeed = 0u;
	uint				max_dashSpeed = 0u;
	bool				canDash = false;

	//Speeds
	uint				jumpSpeed = 0u;
	uint				max_jumpSpeed = 0u;
	uint				fallSpeed = 0u;
	uint				max_FallSpeed = 0u;
	uint				gravityForce = 0u;
	uint				max_gravityForce = 0u;
	uint				runSpeed = 0u;


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



	//Rects
	SDL_Rect* rect = nullptr;

	//Object Layer
	ObjectLayer* lay = nullptr;
	


	//XML Stuff
	pugi::xml_document	player_file;
	p2SString			folder;
	bool				map_loaded = false;


	p2Point<int> relativePos = { 0,0 };
};


#endif // __j1PLAYER_H__