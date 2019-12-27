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
	void UpdateUI();

	UI_Element* CreateUIImage(SDL_Rect image, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,UI_Element* parent = nullptr);
	UI_Element* CreateUIButton(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type, UI_Element* parent = nullptr);
	p2List<UI_Element*> UI_Elements_list;


	/* MAIN MENU*/

	//Images
	UI_Element* backgroundImage = nullptr;

	//Buttons
	UI_Element* playButton = nullptr;
	UI_Element* continueButton = nullptr;
	UI_Element* creditsButton = nullptr;
	UI_Element* settingsButton = nullptr;
	UI_Element* quitButton = nullptr;
};

#endif // !__J1UI_H__

