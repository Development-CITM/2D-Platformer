#ifndef __J1UI_H__
#define __J1UI_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"
#include "UI_Element.h"


class j1UI : public j1Module
{
public:
	j1UI();
	virtual ~j1UI();


	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	void Draw();

	void CreateUIElement(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type);
	p2List<UI_Element*> UI_Elements_list;
};

#endif // !__J1UI_H__

