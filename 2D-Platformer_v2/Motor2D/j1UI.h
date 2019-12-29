#ifndef __J1UI_H__
#define __J1UI_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Fonts.h"
enum ButtonType;

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

	bool quit = false;

	UI_Element* CreateUIImage(SDL_Rect image, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,UI_Element* parent = nullptr);
	UI_Element* CreateUIButton(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,ButtonType button, UI_Element* parent = nullptr);
	void CreateNewScoreFont(UI_Element* parent);
	void CreateNewTimerFont(UI_Element* parent);
	UI_Fonts* CreateUIText(const char* text,p2Point<int> offset,p2Point<int> screen, UI_Element* parent = nullptr);
	UI_Fonts* CreateUIText(UI_Element* parent = nullptr);
	void AddScore();
	p2List<UI_Element*> UI_Elements_list;

	/*----HUD----*/
	UI_Element* coin_background = nullptr;
	UI_Element* coin_image = nullptr;
	UI_Element* timer_image = nullptr;
	UI_Element* timer_background = nullptr;

	/* ----MAIN MENU----*/
	//Images
	UI_Element* backgroundImage = nullptr;

	//Buttons
	UI_Button* playButton = nullptr;
	UI_Button* continueButton = nullptr;
	UI_Button* creditsButton = nullptr;
	UI_Button* settingsButton = nullptr;
	UI_Button* quitButton = nullptr;

	/*----PAUSE MENU----*/




	/*------SETTINGS------*/
	//Background
	UI_Element* settingsBackground = nullptr;

	//Buttons
	UI_Button* muteButton = nullptr;
	UI_Button* soundButton = nullptr;
	UI_Button* returnButton = nullptr;

	/*---CREDITS---*/
	UI_Element* creditsImage = nullptr;


	/*Text*/
	UI_Element* image_font = nullptr;
	UI_Fonts* font = nullptr;
};

#endif // !__J1UI_H__

