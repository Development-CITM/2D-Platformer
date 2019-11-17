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

enum CharacterState {
	ST_Idle,
	ST_Walk,
	ST_Run,
	ST_Jump,
	ST_DoubleJump,
	ST_MP,
	ST_LK,
	ST_Fall,
	ST_GrabLedge,
};

enum class Directions
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
	SDL_Texture*			texture = nullptr;
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

	void Gravity();

	void ChangeStates();

	// Called before all Updates
	bool PostUpdate();

	// Called each loop iteration if has one
	bool Load(const char* path);

	// Called each loop iteration if has one
	void Draw();

	// Called before quitting
	bool CleanUp();


	void OnCollision(Collider* c1, Collider* c2) override;

	Collider* GetCollider() { return player_Collider; }

	void SetPlayerPos(pugi::xml_node& object);

private:

	//Load Functions
	bool LoadPlayerTMX(pugi::xml_node& player_node);
	Animation* LoadAnimation(pugi::xml_node& obj_group);
	SDL_Rect LoadAABB(pugi::xml_node& player_node);

	//Animation Functions
	void ChangeAnimation(Animation*);

	//Logical Functions
	void UpdatePlayerPosition();

	void UpdateCheckersPosition();

	void UpdateCheckersBools();

	void ChangeStatesAndAnimations();

	void JumpMove(float dt);

	void HorizontalMove(float);

//---------------VARIABLES --------------------//
public:
	PlayerTMXData		player_tmx_data;

	p2Point<int>		playerPos{ 0,0 };

private:
	//Run Speeds
	float				runSpeed = 2.f;
	float				max_runSpeed = 2.f;
	float				gravitySpeed = 0.4f;

	//Jump Speed
	float				max_gravitySpeed = 0.7f;
	float				jumpSpeed = -7.f;
	float				double_jumpSpeed = -5.f;
	float				verticalSpeed = 0.f;

	//Move bools
	bool				moveRight = false;
	bool				moveLeft = false;
	bool				jumpPressed = false;
	bool				canDoubleJump = true;
	bool				onGround = false;
	bool				canMove = true;
	bool				atCeiling = false;
	bool				jumping = false;
	bool				falling = true;
	bool				doubleJumped = false;

	//Jump ints
	int					jumpCount = 0;


	//Enums
	SDL_RendererFlip	flip = SDL_RendererFlip::SDL_FLIP_NONE;

	CharacterState		state = CharacterState::ST_Idle;
	CharacterState		previous_state = CharacterState::ST_Idle;

	//Actual movement speed  direction
	int					velocity_X = 0;
	int					velocity_Y = 0;

	//Positions
	bool				onPlatform = false;
	p2Point<int>		currentVelocity{ 0,0 };

	//Animations
	Animation*			currentAnimation = nullptr;
	Animation*			previousAnimation = nullptr;

	Animation*			disarmed_idle = nullptr;
	Animation*			disarmed_run = nullptr;
	Animation*			disarmed_jump = nullptr;
	Animation*			disarmed_fall = nullptr;
	Animation*			disarmed_double_jump = nullptr;

	//Attacks
	Animation*			disarmed_mp = nullptr;
	Animation*			disarmed_lk = nullptr;

	int					numCurrentAnimation = 0;

	//Colliders
	Collider*			player_Collider = nullptr;

	//Checkers
	Collider*			ceilingChecker = nullptr;
	Collider*			groundChecker = nullptr;
	Collider*			leftChecker = nullptr;
	Collider*			rightChecker = nullptr;

	//Offsets
	int					colliderOffsetY1 = 10;
	int					colliderOffsetY2 = 9;

	//XML Stuff
	p2SString			folder;
	bool				map_loaded = false;
};


#endif // __j1PLAYER_H__