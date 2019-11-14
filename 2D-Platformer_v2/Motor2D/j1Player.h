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
	Idle,
	Walk,
	Run,
	Jump,
	Fall,
	GrabLedge,
};

enum MoveDirection {
	None,
	Right,
	Left,
	Up,
	Down,
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

	void RoundPosition();

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

	void MovePlayerX(float value);
	void MovePlayerY(float value);
	//void UpdatePhysics();
	//void UpdateAABB();
	//void CharacterUpdate();

	void HorizontalMove();
	void JumpMove();
	void Gravity();
	void GroundCheck();
	void ChangeStates();

	//bool CheckCollision();

	bool MoveToDirection(MoveDirection);
	//STATE MACHINE
	//bool StateIdle();
	//bool StateRun();
	//bool StateJump();
	//bool StateFall();


//---------------VARIABLES --------------------//
public:
	PlayerTMXData		player_tmx_data;
	int					groundPos;

	p2Point<float> mPosition{ 0,0 };
	p2Point<int> mPositionRounded{ 0,0 };
private:

	//Enums

	SDL_RendererFlip	flip = SDL_FLIP_NONE;

	MoveDirection		moveDirection = MoveDirection::None;

	//Positions

	CharacterState		mCurrentState = CharacterState::Idle;
	float				mWalkSpeed = 0.0f;
	float				mRunSpeed = 2.f;
	float				mJumpSpeed = 0.f;
	float				mMaxJumpSpeed = -12.f;
	float				mRunAcceleration = 0.0f;
	float				timer = 0.0f;
	float				delayToJump = 0.f;
	float				cGravity = 0.5f;
	bool				startJump = false;
	//Animations
	Animation*			mAnimation = nullptr;

	Animation*			disarmed_idle = nullptr;
	Animation*			disarmed_run = nullptr;
	Animation*			disarmed_jump = nullptr;
	Animation*			disarmed_fall = nullptr;


	//Checkers
	bool				detected_Collision = false;


	bool				mGoRight = false;
	bool				mGoLeft = false;
	bool				mJump = false;
	bool				falling = true;

	//Colliders
	Collider*			AABB_current = nullptr;
	Collider*			AABB_previous = nullptr;

	//XML Stuff
	p2SString			folder;
	bool				map_loaded = false;

	 p2Point<float> mOldPosition;


	 p2Point<float> mOldSpeed;
	 p2Point<float> mSpeed;


	 bool mOnGround = false;


	 int numCurrentAnimation = 0;

	 

};


#endif // __j1PLAYER_H__