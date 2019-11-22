#ifndef _ENEMIESOBJECTS_H
#define _ENEMIESOBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"


class Object_Enemy : public Object_Character {

public:

	Object_Enemy(Object_type type);
	~Object_Enemy();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	Animation* LoadAnimation(pugi::xml_node& obj_group);
	SDL_Rect LoadAABB(pugi::xml_node& AABB_object);
public:
	int aha;
	Animation* idle;
	Animation* running;
	Animation* hurt;
	Animation* dead;




};

#endif