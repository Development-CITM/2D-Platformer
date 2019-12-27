#include "UI_Button.h"
#include "j1App.h"
#include "j1Input.h"
#include "SDL/include/SDL_mouse.h"
#include "p2Log.h"

UI_Button::UI_Button(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type, UI_Element* parent) : hover_rect(hover), pressed_rect(pressed), isHover(false), isPressed(false),isEnabled(true), UI_Element(image, text, offset, { 0,0 }, screen, ui_type, parent)
{
	
}

UI_Button::~UI_Button()
{
}

void UI_Button::Update()
{
	if (UI_Type != TYPE::UI_Button)
		return;

	int x, y;

	App->input->GetMousePosition(x, y);
	LOG("(%i,%i)", x, y);

	if (x > screenPos.x&& x < screenPos.x + activeRect.w && y > screenPos.y&& y < screenPos.y + activeRect.h && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT)) {

		isPressed = true;
		activeRect = pressed_rect;
	}
	else if (x > screenPos.x&& x < screenPos.x + activeRect.w && y > screenPos.y&& y < screenPos.y + activeRect.h)
	{
		isPressed = false;
		isHover = true;
		activeRect = hover_rect;
	}
	else {
		isHover = false;
		isPressed = false;
		activeRect = base_rect;
	}

}

void UI_Button::SetRectToShow(SDL_Rect rect)
{
	SetBaseRect(rect);
}
