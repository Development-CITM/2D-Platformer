#ifndef __j1COLLIDERS_H__
#define __j1COLLIDERS_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum ColliderType
{
	COLLIDER_NONE,
	COLLIDER_PLAYER,
	COLLIDER_WALL_SOLID,
	COLLIDER_WALL_TRASPASSABLE,
	COLLIDER_DEAD,
	COLLIDER_GROUND_CHECKER,
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	j1Module* callback = nullptr;
	bool Enabled = true;
	p2Point<int> GetPosition() { return { rect.x,rect.y }; }

	int ColliderDamage = 0;

	ColliderType type = COLLIDER_NONE;
	p2Point<int> offset = { 0, 0};

	Collider() {
		ColliderDamage = 0;
		rect = { 0,0,0,0 };
		type = COLLIDER_NONE;
	}
	Collider(SDL_Rect rectangle, ColliderType type, p2Point<int> offset = { 0, 0}, j1Module* callback = nullptr, int Damage = 0) :
		rect(rectangle),
		offset(offset),
		type(type),
		callback(callback),
		ColliderDamage(Damage) {

	}


	void MoveCollider(p2Point<int> pos) {
		rect.x = pos.x;
		rect.y = pos.y;

	}

	bool Collider::CheckCollision(const SDL_Rect& r) const;
};

class j1Colliders : public j1Module
{
public:
	j1Colliders();

	// Destructor
	virtual ~j1Colliders();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Load(pugi::xml_node object);

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	//Loads all collider objects
	bool LoadObject(pugi::xml_node& node, Collider* collider);

	//Collider* CreateCollider(SDL_Rect* rect,p2Point<int> pos ,int type);
	Collider* AddCollider(SDL_Rect rect, ColliderType type, p2Point<int> offset = { 0, 0 },j1Module * callback = nullptr, int Damage = 0);


private:
	bool CheckCollisionTypes(Collider* c1, Collider* c2);

public:

	bool collider_debug = false;
	
private:
	
	p2List<Collider*>	colliders;

	bool collider_loaded = false;
	int scale;
	int size;

	
	bool** collider_matrix;

};
#endif
