#ifndef _PLAYEROBJECTS_H
#define _PLAYEROBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1GameObjects.h"
#include "DynamicObjects.h"
#include "CharacterObjects.h"


class Object_Player : public Object_Character {
public:
	Object_Player(int var);
	~Object_Player();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
public:

	int aha;


};
#endif // !_PLAYEROBJECTS_H
