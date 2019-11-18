#ifndef _DYNAMICOBJECTS_H
#define _DYNAMICOBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"

class Object_Character;
class Object_Map_Object;

class Object_Dynamic : public  EntityManager {

public:
	Object_Dynamic();
	Object_Dynamic(Dynamic_type type);
	~Object_Dynamic();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	void CreateCharacter();
public:
	p2List<Object_Character*> character_objects;
};

#endif

