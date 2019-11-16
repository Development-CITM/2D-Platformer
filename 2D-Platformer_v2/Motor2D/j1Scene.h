#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

struct Cameralimit
{
	SDL_Rect rect;
};


class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Called before quitting
	bool CleanUp();

	bool LoadSceneLimits(pugi::xml_node object);

	void DecideMapToLoad();

	void ColliderMapToLoad(Collider* c2);
	
private:

private:
	
	pugi::xml_document save_game_file;

	//pugis
	pugi::xml_parse_result result;
	pugi::xml_node renderer;

	
public:

	bool notfirst=false;

	bool loading = false;
	bool swapping = false;
	p2List<p2SString*> levels;
	p2SString A1 = "maps/A1.tmx";
	p2SString A2 = "maps/A2.tmx";
	p2SString A3 = "maps/A3.tmx";
	p2SString A5 = "maps/A5.tmx";
	p2SString A6 = "maps/A6.tmx";
	p2SString current_level;
	p2SString destination_level = "maps/A2.tmx";

	int camera_limit_left = 0;
	int camera_limit_right = 0;

	bool hasExit = false;

	Collider* exitCollider = nullptr;

};

#endif // __j1SCENE_H__