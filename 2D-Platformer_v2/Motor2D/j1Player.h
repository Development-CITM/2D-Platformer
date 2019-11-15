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

enum CharacterState {
	ST_Idle,
	ST_Walk,
	ST_Run,
	ST_Jump,
	ST_Fall,
	ST_GrabLedge,
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


struct ObjectLayer
{
	p2SString			name;
	p2List<SDL_Rect*>	rects;
};

struct PlayerTMXData {
	SDL_Texture* texture = nullptr;
	uint					width = 0u;
	uint					height = 0u;
	int						tile_width = 0u;
	int						tile_height = 0u;
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

	void ReSizeAABBFromAnimation();



	// Called before all Updates
	bool PostUpdate();

	// Called each loop iteration if has one
	bool Load(const char* path);


	// Called each loop iteration if has one
	void Draw();

	// Called before quitting
	bool CleanUp();

	void SetPlayerPos(pugi::xml_node& object);
	Collider* GetPlayerCollider() const { return player_Collider; }
	int GetVelocityX() { return velocity_X; }

	void OnCollision(Collider* c1, Collider* c2) override;

	void				ResetInputs();
private:
	p2Point<int> relativePos = { 0,0 };
	//Inputs
	void HorizontalInput();
	void VerticalInput();

	//void DashInput();

	void ExitInput();

	//Habilities
	//void Dash();

	//Logic Movements
	void Move();
	void Jump();
	void Gravity();

	bool MoveTo(Directions dir);

	//Load Functions
	bool LoadPlayerTMX(pugi::xml_node& player_node);
	Animation* LoadAnimation(pugi::xml_node& obj_group);
	SDL_Rect LoadAABB(pugi::xml_node& player_node);

	//Animation Functions
	void ChangeAnimation(Animation*);

//---------------VARIABLES --------------------//
public:
	PlayerTMXData		player_tmx_data;
	int					groundPos;

	p2Point<int> playerPos{ 0,0 };

private:

	//Heights for adjustment while DIR_DOWN
	int					playerheight_dir_down = 0;
	int					colliderheight_dir_down = 0;

	bool				canMove = true;
	//Enums
	bool				jumping = false;
	SDL_RendererFlip	flip = SDL_FLIP_NONE;
	//Actual movement speed  direction
	int					velocity_X = 0;
	int					velocity_Y = 0;

	Directions			direction = DIR_RIGHT;

	Directions			last_Direction = DIR_RIGHT;

	//Positions
		//Positions
	int					pivot_x_flip = 0;
	bool				onPlatform = false;
	p2Point<int>		previousColliderPos = { 0,0 };
	p2Point<int>		previousPlayerPos{ 0,0 };
	p2Point<int>		currentVelocity{ 0,0 };

	CharacterState		state = ST_Idle;
	int					mWalkSpeed = 0;
	int					mRunSpeed = 2;
	int					mJumpSpeed = 0;
	int					mMaxJumpSpeed = -12;
	int					mRunAcceleration = 0;
	int					timer = 0;
	int					delayToJump = 0;
	int					cGravity = 1;
	bool				startJump = false;

	//Jump Variables
	uint				maxJump = 0u;
	uint				jumpDistance = 0u;
	uint				timeOnAir = 0u;
	uint				currentTimeAir = 0u;


	bool				move_To_Right = false;
	bool				move_To_Left = false;
	bool				move_To_Up = false;
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
	uint				max_gravityForce = 5u;
	uint				runSpeed = 0u;


	//Animations
	//Animations
	Animation*			currentAnimation = nullptr;
	Animation*			previousAnimation = nullptr;

	Animation*			disarmed_idle = nullptr;
	Animation*			disarmed_run = nullptr;
	Animation*			disarmed_jump = nullptr;
	Animation*			disarmed_fall = nullptr;

	//Checkers
	bool				detected_Collision = false;

	//Colliders
	Collider*			player_Collider = nullptr;


	//XML Stuff
	p2SString			folder;
	bool				map_loaded = false;

	 p2Point<float> mOldSpeed;
	 p2Point<float> mSpeed;


	 bool onGround = false;


	 int numCurrentAnimation = 0;

	 

};


#endif // __j1PLAYER_H__