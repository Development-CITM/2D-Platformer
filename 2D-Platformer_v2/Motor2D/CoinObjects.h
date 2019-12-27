#ifndef _COINOBJECTS_H
#define _COINOBJECTS_H

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1EntityManager.h"
#include "CharacterObjects.h"

class Object_Coin : public Object_Character {

public:
	Object_Coin(pugi::xml_node& object);
	~Object_Coin();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

private:
	virtual Animation* LoadAnimation(pugi::xml_node& obj_group);
	void SetPos(pugi::xml_node& object);

public:
	Animation* spin;
	
	int			numCurrentAnimation = 0;
};



#endif // !_COINOBJECTS_H
