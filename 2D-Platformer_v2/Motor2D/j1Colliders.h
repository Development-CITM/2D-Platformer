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
	COLLIDER_WINDOW,
	COLLIDER_CAMERA,
	COLLIDER_EXIT,
};

struct Collider
{
	SDL_Rect		rect{ 0,0,0,0 };
	p2Point<int>	offset = { 0, 0};
	j1Module*		callback = nullptr;
	bool			to_delete = false;
	bool			Enabled = true;
	int				ColliderDamage = 0;
	ColliderType	type = COLLIDER_NONE;

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
	void Resize(SDL_Rect rect_target) {
		rect = rect_target;
	}

	void MoveCollider(p2Point<int> pos) {
		rect.x = pos.x;
		rect.y = pos.y;
	}

	p2Point<int> GetPosition() { return { rect.x,rect.y }; }

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

	bool Load(pugi::xml_node &object);

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

	//Add collider
	Collider* AddCollider(SDL_Rect rect, ColliderType type, p2Point<int> offset = { 0, 0 },j1Module * callback = nullptr, int Damage = 0);

public:

	bool CheckColliderCollision(Collider*, int* posY = nullptr);
	bool ThroughPlatform (Collider*);
	bool collider_debug = false;
	
private:
	Collider*			collider = nullptr;
	Collider*			c = nullptr;
	p2List<Collider*>	colliders;
	p2List<Collider*>	detected_Colliders;

	bool				collider_loaded = false;

	int					scale = 0;
	int					size = 0;
};
#endif
