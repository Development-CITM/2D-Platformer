#ifndef _ENEMIESOBJECTS_H
#define _ENEMIESOBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1EntityManager.h"
#include "CharacterObjects.h"

//Enemy particular states
enum class State {
	IDLE,
	RUNNING,
	ATTACK,
	DIE,
	FALL
};

class Object_Enemy : public Object_Character {

public:

	Object_Enemy(Object_type type, p2Point<int> posInit = { 0,0 });

	~Object_Enemy();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	//Creates a path with origin enemy and target player
	void StablishPath();

	//Enemy moves to path target
	void MoveToTarget(p2Point<int>);

	//Loads animations for each enemy type
	Animation* LoadAnimation(pugi::xml_node& obj_group);

	//Changes animations
	void ChangeAnimation(Animation*);

	//Init checkers for each direction
	bool InitCheckers(Object_type type);

public:

	//Condition to not be loading enemies tmx for each one
	bool firstkobold = true;
	bool firstwhisp = true;

	//Enemies animation types
	Animation* idle;
	Animation* running;
	Animation* hurt;
	Animation* dead;
	Animation* attack;

	//Timing for enemy attacks
	int attackDelay = 100;
	int timer = 0;

	//Conditions for enemies
	bool canAttack = true;
	bool moving = false;
	bool onGround = true;

	//Current enemy state
	State state;

	//Checker for enemy_ground falling
	Collider* groundChecker = nullptr;
	Collider* collider_test = nullptr;
	Collider* enemyHit = nullptr;

	float deltavar = 0;

	bool inRange = false;

};

#endif