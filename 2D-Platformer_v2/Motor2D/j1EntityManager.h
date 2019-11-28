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
class Object_Static;
class Object_Dynamic;


class GameObject {
	friend class j1EntityManager; //This way we can access Objects functions through the functions of GameObjects class which should be protected
private:
	virtual bool Start();
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();
	virtual bool CleanUp();
public:

	SDL_RendererFlip	flip = SDL_RendererFlip::SDL_FLIP_NONE;
	Collider* collider = nullptr;
	p2Point<int> position;
	Object_type type_object;

	//Maybe we should think about a function which updates all entities through dt
	//We need to create Game Objects and also destroy them. Functions to create static/dynamic and generically destroy game objects
	//We need to check all collisions between objects so function 'OnCollision' or it's homologus should be in here
	//All objects states need to be saved into the XML File so exclusive save and load for objects should be in here
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

	void CreatePlayer();
	void DestroyEnemies();
	void CreateEnemy(p2Point<int> pos, Object_type type);
	void LoadEnemiesFromMap(pugi::xml_node& object);
	void LoadEnemiesFromBackup();

public:
	p2List<GameObject*>  objects;
	p2List<GameObject*> backup;
};

#endif // 
