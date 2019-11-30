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

	void LogicStateMachine(float dt);

	void JumpStart(float dt);

	void DoubleJumpStart(float dt);

	void Gravity(float dt);

	void ChangeStates();

	bool PostUpdate();

	bool InitCheckers();

	void SetPos(pugi::xml_node& object);
	void SetPlayerPosFromCurrentLevel(pugi::xml_node& it);
private:
	Animation* LoadAnimation(pugi::xml_node& obj_group);

	SDL_Rect LoadAABB(pugi::xml_node& AABB_object);

	void ChangeAnimation(Animation*);

	void HorizontalMove(float dt);

	void UpdatePlayerPosition();

	void UpdateCheckersPosition();

	void UpdateCheckersBools();

	void UpdateColliderSize();

	void JumpInput();
	void HorizontalInputs();
	void AttackInputs();


public:
	//Move bools
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


	int					colliderOffsetY1 = 10;
	int					colliderOffsetY2 = 9;

	float				dt_variable;


};
#endif // !_PLAYEROBJECTS_H
