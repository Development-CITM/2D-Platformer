#ifndef _PLAYEROBJECTS_H
#define _PLAYEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"
#include "Animation.h"



class Object_Player : public Object_Character {
public:
	Object_Player(int var);
	~Object_Player();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool InitCheckers();
	Animation* LoadAnimation(pugi::xml_node& obj_group);
public:

private:
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

	//Animations
	Animation* disarmed_idle = nullptr;
	Animation* disarmed_run = nullptr;
	Animation* disarmed_jump = nullptr;
	Animation* disarmed_fall = nullptr;
	Animation* disarmed_double_jump = nullptr;

	//Attacks
	Animation* disarmed_mp = nullptr;
	Animation* disarmed_lk = nullptr;

	//Colliders
	Collider* player_Collider = nullptr;

	//Checkers
	Collider* ceilingChecker = nullptr;
	Collider* groundChecker = nullptr;
	Collider* leftChecker = nullptr;
	Collider* rightChecker = nullptr;

	int					colliderOffsetY1 = 10;
	int					colliderOffsetY2 = 9;
	int aha;


};
#endif // !_PLAYEROBJECTS_H
