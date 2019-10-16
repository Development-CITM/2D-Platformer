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
	COLLIDER_DEAD
};

struct Collider
{
	SDL_Rect rect;
	ColliderType type;
	p2Point<int> offset = { 0, 0};

	Collider() {
		rect = { 0,0,0,0 };
		type = COLLIDER_NONE;
	}

	void MoveCollider(p2Point<int> pos) {
		rect.x = pos.x + offset.x;
		rect.y = pos.y + offset.y;

	}

	bool Collider::CheckCollision(const SDL_Rect& r) const;
	bool Collider::OnCollider(j1Module* module,Collider* collider);
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

	Collider* CreateCollider(SDL_Rect* rect,p2Point<int> pos ,int type);

	void SetMatrix();


public:

	bool collider_debug = false;
	
private:
	
	p2List<Collider*>	colliders;
	p2List_item<Collider*>* collider = nullptr;
	bool collider_loaded = false;
	int scale;
	int size;

	
	bool** collider_matrix;

};
#endif
