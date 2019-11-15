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

	void JumpInput();

	void HorizontalInputs();

	// Called each loop iteration
	bool Update(float dt);

	void JumpMove();

	void HorizontalMove();

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


	void OnCollision(Collider* c1, Collider* c2) override;

private:
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

	bool				jumping = false;
	p2Point<int> playerPos{ 0,0 };

	float				verticalSpeed = 0.f;
	bool				atCeiling = false;

	int					colliderOffsetY1 = 10;
	int					colliderOffsetY2 = 9;

private:
	float				runSpeed = 2.f;
	float				gravitySpeed = 0.5f;

	bool				moveRight = false;
	bool				moveLeft = false;
	bool				jumpPressed = false;
	bool				onGround = false;
	bool				canMove = true;


	//Enums
	SDL_RendererFlip	flip = SDL_RendererFlip::SDL_FLIP_NONE;

	//Actual movement speed  direction
	int					velocity_X = 0;
	int					velocity_Y = 0;

	Directions			direction = Directions::DIR_RIGHT;

	Directions			last_Direction = Directions::DIR_RIGHT;

	//Positions
		//Positions
	int					pivot_x_flip = 0;
	bool				onPlatform = false;
	p2Point<int>		previousColliderPos = { 0,0 };
	p2Point<int>		previousPlayerPos{ 0,0 };
	p2Point<int>		currentVelocity{ 0,0 };

	CharacterState		state = CharacterState::ST_Idle;

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
	Collider*			ceilingChecker = nullptr;


	//XML Stuff
	p2SString			folder;
	bool				map_loaded = false;


	int numCurrentAnimation = 0;

	 

};


#endif // __j1PLAYER_H__