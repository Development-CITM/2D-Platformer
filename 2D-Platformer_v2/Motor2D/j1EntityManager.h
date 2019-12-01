#ifndef _j1GameObjects_H
#define _j1GameObjects_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Render.h"
#include "j1App.h"
#include "Brofiler/Brofiler.h"

//Map types_v2 to avoid confflict while traslating
enum Object_type {
	PLAYER,
	ENEMY_GROUND,
	ENEMY_FLYING,
};

//Map types_v2 to avoid confflict while traslating
enum MapTypes_v2
{
	MAPTYPE_UNKNOWN_v2 = 0,
	MAPTYPE_ORTHOGONAL_v2,
	MAPTYPE_ISOMETRIC_v2,
	MAPTYPE_STAGGERED_v2
};

//Object layer structs
struct ObjectLayer_v2
{
	p2SString			name;
	p2List<SDL_Rect*>	rects;
};

//TMXData for each object since we load them from tmx
struct CharacterTMXData {
	SDL_Texture* texture = nullptr;
	uint					width = 0u;
	uint					height = 0u;
	int						tile_width = 0u;
	int						tile_height = 0u;
	MapTypes_v2				type;
	p2List<ObjectLayer_v2*>	object_Layers;
};

//First fork between static and dynamic objects. Both not required for this assignment
class Object_Static;
class Object_Dynamic;


class GameObject {

	friend class j1EntityManager; 

	virtual bool Start();

	virtual bool PreUpdate();

	virtual bool Update(float dt);

	virtual bool PostUpdate();

	virtual bool CleanUp();

public:
	//Gets the asked collider
	Collider* GetCollider() { return collider; }

public:

	//Each object flip condition
	SDL_RendererFlip	flip = SDL_RendererFlip::SDL_FLIP_NONE;

	//Each object collider
	Collider*			collider = nullptr;

	//Each object position
	p2Point<int>		position;

	//Each object type
	Object_type		    type_object;

	//On map?
	bool				alive;

	//Each object data loaded from tmx
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

	//Destroys all enemies
	void DestroyEnemies();

	//Destroys a dead enemy
	void DestroyEnemyDead();

	//Destroys player
	void DestroyPlayer();

	//CreatesEnemy
	void CreateEnemy(p2Point<int> pos, Object_type type);

	//Fills a temp list to backup enemies
	void FillBackup(p2Point<int> pos, Object_type type);

	//Cleans temp backup list
	void ClearBackup();

	//Loads enemies from map
	void LoadEnemiesFromMap(pugi::xml_node& object);

	//Loads enemies from backup list
	void LoadEnemiesFromBackup();

	//Creates player
	void CreatePlayer(pugi::xml_node& object);

	//Returns exclusively player collider ( for purpose )
	Collider* RetreivePlayerCollider();

	//Returns exclusively player data   ( for purpose )
	CharacterTMXData RetreivePlayerData();



public:
	//Game objects lists
	p2List<GameObject*>  objects;
	p2List<GameObject*> backup;
};

#endif // 
