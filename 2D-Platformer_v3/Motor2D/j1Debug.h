#ifndef __j1DEBUG_H__
#define __j1DEBUG_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

class j1Debug :public j1Module {

public:

	j1Debug();
	~j1Debug();
	bool Update(float dt);

public:
	bool input = true;
};
#endif

