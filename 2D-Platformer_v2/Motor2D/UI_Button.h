#ifndef __UI_Button_H__
#define __UI_Button_H__
#include "UI_Element.h"

class UI_Button : public UI_Element
{
public:
	UI_Button(SDL_Rect image,SDL_Rect hover,SDL_Rect pressed, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type, UI_Element* parent);
	virtual ~UI_Button();

	SDL_Rect hover_rect;
	SDL_Rect pressed_rect;
	
	bool isEnabled;
	bool isPressed;
	bool isHover;

	void Update() override;

	void SetRectToShow(SDL_Rect rect);
};

#endif // !__UI_Button_H__

