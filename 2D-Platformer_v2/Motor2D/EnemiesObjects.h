#ifndef _ENEMIESOBJECTS_H
#define _ENEMIESOBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1EntityManager.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"


class Object_Enemy : public Object_Character {

public:

	Object_Enemy(Object_type type, p2Point<int> posInit = { 0,0 });
	~Object_Enemy();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void StablishPath();
	void MoveToTarget(p2Point<int>);
	Animation* LoadAnimation(pugi::xml_node& obj_group);
public:
	bool firstkobold = true;
	bool firstwhisp = true;
	Animation* idle;
	Animation* running;
	Animation* hurt;
	Animation* dead;
	Animation* attack;

	bool moving = false;

	bool alive = true;


};

#endif