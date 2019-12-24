#include "UI_Button.h"

UI_Button::UI_Button(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type) : hover_rect(hover),pressed_rect(pressed),isHover(false),isPressed(false), UI_Element(image,text,local,screen,ui_type)
{
	
}

UI_Button::~UI_Button()
{
}

void UI_Button::SetRectToShow(SDL_Rect rect)
{
	SetBaseRect(rect);
}
