#ifndef _PLAYEROBJECTS_H
#define _PLAYEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1EntityManager.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "Animation.h"

class Object_Player : public Object_Character {
public:
	Object_Player(pugi::xml_node &object);

	~Object_Player();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool CleanUp();

	bool PostUpdate();

	//State machine
	void LogicStateMachine(float dt);

	//Jump start logic
	void JumpStart(float dt);

	//Double jump start logic
	void DoubleJumpStart(float dt);

	//Gravity effect on velocity
	void Gravity(float dt);

	//States changing function
	void ChangeStates();

	//Inits checkers for player in each direction
	bool InitCheckers();

	//Sets player position
	void SetPos(pugi::xml_node& object);

	//Sets player position depending on the access to the zone
	void SetPlayerPosFromCurrentLevel(pugi::xml_node& it);

private:
	//Loads all animations from tmx
	Animation* LoadAnimation(pugi::xml_node& obj_group);

	//Loads all AABB from tmx
	SDL_Rect LoadAABB(pugi::xml_node& AABB_object);

	//Changes animation
	void ChangeAnimation(Animation*);

	//HorizontalMove logic
	void HorizontalMove(float dt);

	//Updates the player position and its collider
	void UpdatePlayerPosition();

	//Updates the checkers in all directions 
	void UpdateCheckersPosition();

	//Updates the current state of the checkers
	void UpdateCheckersBools();

	//Modifies player collider depending on his animation
	void UpdateColliderSize();

	//Inputs for jumping
	void JumpInput();

	//Inputs for horizontal movement
	void HorizontalInputs();

	//Inputs for attacking
	void AttackInputs();


public:

	//Moving and attacking bools
	bool				moveRight = false;
	bool				moveLeft = false;
	bool				jumpPressed = false;
	bool				MP_attackPressed = false;
	bool				LK_attackPressed = false;

	bool				canDoubleJump = true;
	bool				onGround = false;
	bool				canMove = true;
	bool				atCeiling = false;
	bool				jumping = false;
	bool				falling = true;
	bool				doubleJumped = false;
	bool				dead_once = true;

	//Jump ints
	int					doublejumpCount = 0;

	//Checkers
	Collider* ceilingChecker = nullptr;
	Collider* groundChecker = nullptr;
	Collider* leftChecker = nullptr;
	Collider* rightChecker = nullptr;

	//Hit Collider
	Collider* collider_attack = nullptr;


	//Animations
	Animation* disarmed_idle = nullptr;
	Animation* disarmed_run = nullptr;
	Animation* disarmed_jump = nullptr;
	Animation* disarmed_fall = nullptr;
	Animation* disarmed_double_jump = nullptr;
	Animation* disarmed_dead = nullptr;

	//Attacks
	Animation* disarmed_mp = nullptr;
	Animation* disarmed_lk = nullptr;

	//Run Speeds
	float				runSpeed_v2 = 2.f;
	float				max_runSpeed_v2 = 2.f;
	float				gravitySpeed_v2 = 0.4f;

	//Jump Speed
	float				max_gravitySpeed_v2 = 0.7f;
	float				jumpSpeed_v2 = -6.f;
	float				double_jumpSpeed_v2 = -6.f;
	float				verticalSpeed_v2 = 0.f;
	float				max_verticalSpeed_v2 = 6.f;

	//Collider offsets
	int					colliderOffsetY1 = 10;
	int					colliderOffsetY2 = 9;

	//dt variable filled with dt in order to access to dt in PostUpdate to perform camera following
	float				dt_variable;


};
#endif // !_PLAYEROBJECTS_H
