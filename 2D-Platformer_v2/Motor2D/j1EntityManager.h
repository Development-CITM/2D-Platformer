#ifndef _j1GameObjects_H
#define _j1GameObjects_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Render.h"
#include "j1App.h"

struct FlyingEnemy {

};
struct GroundEnemy {

};

enum Object_type {
	PLAYER,
	ENEMY_GROUND,
	ENEMY_FLYING,
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

class Object_Static;
class Object_Dynamic;


class GameObject {

	friend class j1EntityManager; 
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();
	virtual bool CleanUp();
	virtual void SetPos(pugi::xml_node& object);

public:
	Collider* GetCollider() { return collider; }

public:

	SDL_RendererFlip	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	Collider*			collider = nullptr;
	p2Point<int>		position;
	Object_type		    type_object;
	CharacterTMXData	character_tmx_data;
};


class j1EntityManager :public j1Module
{
public:
	j1EntityManager();
	virtual ~j1EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void DestroyEnemies();

	void DestroyPlayer();

	void CreateEnemy(p2Point<int> pos, Object_type type);

	void FillBackup(p2Point<int> pos, Object_type type);

	void ClearBackup();

	void LoadEnemiesFromMap(pugi::xml_node& object);

	void LoadEnemiesFromBackup();

	void CreatePlayer(pugi::xml_node& object);



	Collider* RetreivePlayerCollider();

	CharacterTMXData RetreivePlayerData();



public:
	p2List<GameObject*>  objects;
	p2List<GameObject*> backup;
};

#endif // 
