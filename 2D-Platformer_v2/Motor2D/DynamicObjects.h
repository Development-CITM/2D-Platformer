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
	~Object_Dynamic();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	void Draw();

};

#endif

