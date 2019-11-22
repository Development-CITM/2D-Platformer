#ifndef _CHARACTEROBJECTS_H
#define _CHARACTEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "Animation.h"
#include "j1Render.h"

class Object_Player;
class Object_Enemy;
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

enum MapTypes_v2
{
	MAPTYPE_UNKNOWN_v2 = 0,
	MAPTYPE_ORTHOGONAL_v2,
	MAPTYPE_ISOMETRIC_v2,
	MAPTYPE_STAGGERED_v2
};

struct ObjectLayer_v2
{
	p2SString			name;
	p2List<SDL_Rect*>	rects;
};

struct CharacterTMXData {
	SDL_Texture* texture = nullptr;
	uint					width = 0u;
	uint					height = 0u;
	int						tile_width = 0u;
	int						tile_height = 0u;
	MapTypes_v2				type;
	p2List<ObjectLayer_v2*>	object_Layers;
};

class Object_Character : public Object_Dynamic{

public:
	Object_Character();
	~Object_Character();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool Load(const char* path);
	void Draw();

private:
	bool LoadCharacterTMX(pugi::xml_node& character_node);
	virtual Animation* LoadAnimation(pugi::xml_node& obj_group);
	virtual SDL_Rect LoadAABB(pugi::xml_node& AABB_object);


public:


	CharacterTMXData		character_tmx_data;
	p2Point<int>		characterPos{ 397,375 };

	CharacterState_v2	state = CharacterState_v2::ST_Idle_v2;
	CharacterState_v2		previous_state = CharacterState_v2::ST_Idle_v2;

	Animation* currentAnimation = nullptr;
	Animation* previousAnimation = nullptr;

	int					numCurrentAnimation = 0;
	
};

#endif // !_CHARACTEROBJECTS_H


