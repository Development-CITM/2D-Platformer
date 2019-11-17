#ifndef __j1COLLIDERS_H__
#define __j1COLLIDERS_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "j1Player.h"

enum ColliderType
{
	COLLIDER_NONE,
	COLLIDER_PLAYER,
	COLLIDER_WALL_SOLID,
	COLLIDER_WALL_TRASPASSABLE,
	COLLIDER_TRANSITION,
	COLLIDER_DEAD,
	COLLIDER_WINDOW,
	COLLIDER_CEILING_CHECKER,
	COLLIDER_EXIT,
};

enum SwapTo
{
	NOWHERE,
	A1_TO_A2,
	A1_TO_A3,
	A2_TO_A1,
	A2_TO_A3,
	A5_TO_A1,
	A5_TO_A6,
	A1_TO_A5,
	A3_TO_A2,
};

enum ColliderChecker {
	None,
	Ground,
	Left,
	Right,
	Top,
};


struct Collider
{
	SDL_Rect		rect{ 0,0,0,0 };
	p2Point<int>	offset = { 0, 0};
	j1Module*		callback = nullptr;
	bool			to_delete = false;
	bool			Enabled = true;
	bool			collided = false;
	int				ColliderDamage = 0;
	ColliderType	type = ColliderType::COLLIDER_NONE;
	SwapTo			swap = SwapTo::NOWHERE;
	ColliderChecker checkerType = ColliderChecker::None;

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
		rect.x += offset.x;
		rect.y += offset.y;

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

	bool CheckColliderCollision(Collider*, Directions dir, int* snapPos = nullptr);
	bool CheckColliderCollision(Collider*);
	void ChooseSwap(pugi::xml_node& node,Collider* c);

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
