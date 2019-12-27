#include "UI_Button.h"

UI_Button::UI_Button(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type, UI_Element* parent) : hover_rect(hover), pressed_rect(pressed), isHover(false), isPressed(false), UI_Element(image, text, offset, { 0,0 }, screen, ui_type, parent)
{
	
}

UI_Button::~UI_Button()
{
}

void UI_Button::SetRectToShow(SDL_Rect rect)
{
	SetBaseRect(rect);
}
