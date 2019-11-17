#ifndef _CHARACTEROBJECTS_H
#define _CHARACTEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"

class Object_Player;
class Object_Enemy;

class Object_Character : public Object_Dynamic{

public:
	Object_Character();
	Object_Character(Character_type type);
	~Object_Character();

private:
	void CreatePlayer();
	p2List<Object_Character*> character_objects;
};

#endif // !_CHARACTEROBJECTS_H


