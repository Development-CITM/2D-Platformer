#ifndef __j1COLLIDERS_H__
#define __j1COLLIDERS_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum ColliderType
{
	BARRIER,
	JUMPABLE,
	DEAD
};

struct Collider
{
	int x;
	int y;
	int width;
	int height;
	ColliderType type;
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

private:
	
	bool LoadObject(pugi::xml_node& node, Collider* collider);
	void ShowColliders();


private:
	
	p2List<Collider*>	colliders;
	p2List_item<Collider*>* collider = nullptr;
	bool collider_debug = false;
	bool collider_loaded = false;

};
#endif
