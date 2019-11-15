#ifndef _j1GameObjects_H
#define _j1GameObjects_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum class_type {
	
	STATIC,
	DYNAMIC,
};
class Object_Static;
class Object_Dynamic;


class EntityManager {
	friend class j1GameObjects; //This way we can access Objects functions through the functions of GameObjects class which should be protected
	

	//Maybe we should think about a function which updates all entities through dt
	//We need to create Game Objects and also destroy them. Functions to create static/dynamic and generically destroy game objects
	//We need to check all collisions between objects so function 'OnCollision' or it's homologus should be in here
	//All objects states need to be saved into the XML File so exclusive save and load for objects should be in here
};


class j1GameObjects :public j1Module
{
public:
	j1GameObjects();
	virtual ~j1GameObjects();

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

public:
	p2List<EntityManager*>* objects;
};

#endif // 
