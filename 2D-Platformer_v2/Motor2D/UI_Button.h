#ifndef __UI_Button_H__
#define __UI_Button_H__
#include "UI_Element.h"
#include "j1Timer.h"

enum ButtonType {
	Start,
	Continue,
	Settings,
	Mute,
	Sound,
	Return,
	Credits,
	Quit,
	MainScreen,
	Github,
};

class UI_Button : public UI_Element
{
public:
	UI_Button(SDL_Rect image,SDL_Rect hover,SDL_Rect pressed, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type, ButtonType button, UI_Element* parent);
	virtual ~UI_Button();

	ButtonType button_type;
	SDL_Rect hover_rect;
	SDL_Rect pressed_rect;
	
	bool isEnabled;
	bool isPressed;
	bool wasPressed;
	bool isHover;
	bool clicked;


	void Update() override;

	void OnClick();

	void SetRectToShow(SDL_Rect rect);
};

#endif // !__UI_Button_H__

