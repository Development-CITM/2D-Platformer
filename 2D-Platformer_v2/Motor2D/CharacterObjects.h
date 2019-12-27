#ifndef _CHARACTEROBJECTS_H
#define _CHARACTEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1EntityManager.h"
#include "Animation.h"
#include "j1Render.h"

class Object_Player;
class Object_Enemy;
class Object_Coin;
class SDL_Texture;


enum CharacterState_v2 {
	ST_Idle_v2,
	ST_Walk_v2,
	ST_Run_v2,
	ST_Jump_v2,
	ST_DoubleJump_v2,
	ST_MP_v2,
	ST_LK_v2,
	ST_Fall_v2,
	ST_GrabLedge_v2,
	ST_Die_v2,
};

enum class Directions_v2
{
	DIR_RIGHT_v2,
	DIR_LEFT_v2,
	DIR_UP_v2,
	DIR_DOWN_v2,
	DIR_PLATFORM_v2,
	DIR_DASH_LEFT_v2,
	DIR_DASH_RIGHT_v2,
	DIR_NONE_v2,
};

class Object_Character : public GameObject {

public:
	Object_Character();
	~Object_Character();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool Load(const char* path);
	void Draw(float dt);

private:
	bool LoadCharacterTMX(pugi::xml_node& character_node);
	virtual Animation* LoadAnimation(pugi::xml_node& obj_group);
	virtual SDL_Rect LoadAABB(pugi::xml_node& AABB_object);
	


public:	
	p2Point<int>			absolutePos;

	int						velocity_X = 0;
	int						velocity_Y = 0;
	p2Point<int>			currentVelocity{ 0,0 };
	bool					onPlatform = false;

	CharacterState_v2		state = CharacterState_v2::ST_Idle_v2;
	CharacterState_v2		previous_state = CharacterState_v2::ST_Idle_v2;

	Animation*				currentAnimation = nullptr;
	Animation*				previousAnimation = nullptr;

	int						numCurrentAnimation = 0;
	
};

#endif // !_CHARACTEROBJECTS_H


