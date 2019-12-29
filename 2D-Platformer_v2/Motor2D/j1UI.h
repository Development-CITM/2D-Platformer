#ifndef __J1UI_H__
#define __J1UI_H__
#include "j1Module.h"
#include "p2List.h"
#include "p2DynArray.h"
#include "UI_Element.h"
#include "UI_Button.h"
#include "UI_Text.h"
#include "UI_InputText.h"
#include "j1Timer.h"

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
	UI_Text* CreateUIText(const char*, p2Point<int> offset, p2Point<int> screen,TextType type = HUD, UI_Element* parent = nullptr);
	UI_InputText* CreateUIInputText(p2Point<int> size, p2Point<int> offset, p2Point<int> screen, UI_Element* parent = nullptr);
	void SubstractLives();
	p2List<UI_Element*> UI_Elements_list;

	/*----HUD----*/
	UI_Element* coin_background = nullptr;
	UI_Element* coin_image = nullptr;
	UI_Element* timer_image = nullptr;
	UI_Element* timer_background = nullptr;

	UI_Element* fiveLives = nullptr;
	UI_Element* fourLives = nullptr;
	UI_Element* threeLives = nullptr;
	UI_Element* twoLives = nullptr;
	UI_Element* oneLives = nullptr;
	UI_Element* zeroLives = nullptr;

	//Text
	UI_Text* score_text = nullptr;
	UI_Text* timer_text = nullptr;

	//Timer
	j1Timer timer;
	j1Timer pause_timer;
	int pause_timer_int = 0;
	int loading_timer_int = 0;
	int timer_int = 0;
	bool resetPause = true;

	/* ----MAIN MENU----*/
	//Images
	UI_Element* backgroundImage = nullptr;

	//Buttons
	UI_Button* playButton = nullptr;
	UI_Button* continueButton = nullptr;
	UI_Button* creditsButton = nullptr;
	UI_Button* settingsButton = nullptr;
	UI_Button* quitButton = nullptr;
	UI_Button* githubButton = nullptr;

	/*----PAUSE MENU----*/
	//Images
	UI_Element* pausemenuBackground = nullptr;

	//Buttons
	UI_Button* pausereturnButton = nullptr;
	UI_Button* pausequitButton = nullptr;



	/*------SETTINGS------*/
	//Background
	UI_Element* settingsBackground = nullptr;

	//Buttons
	UI_Button* muteButton = nullptr;
	UI_Button* soundButton = nullptr;
	UI_Button* returnButton = nullptr;
	UI_Button* creditBackToMenuButton = nullptr;

	/*---CREDITS---*/
	UI_Element* creditsImage = nullptr;


	/*Text*/
	UI_Element* image_font = nullptr;

	/*Lives*/
	int lives = 5;
	bool substract_lives_once = true;

	/*Score*/
	int score = 0;

};

#endif // !__J1UI_H__

