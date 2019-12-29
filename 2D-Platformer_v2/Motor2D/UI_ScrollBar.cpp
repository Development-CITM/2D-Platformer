#include "UI_ScrollBar.h"
#include "j1App.h"
#include "UI_Image.h"
#include "j1Input.h"
#include "UI_Element.h"
#include "p2Log.h"
#include "j1Audio.h"
#include "SDL/include/SDL_mouse.h"
#include "j1UI.h"



UI_ScrollBar::UI_ScrollBar(SDL_Rect image, SDL_Texture* text, p2Point<int> offset_, p2Point<int> screen, TYPE ui_type, UI_Element* parent_ ): UI_Element()
{
	parent = parent_;
	UI_Type = ui_type;
	texture = text;
	base_rect = image;
	activeRect = base_rect;
	offset = offset_;
	screenPos = screen;
}

UI_ScrollBar::~UI_ScrollBar()
{
}

void UI_ScrollBar::Update()
{
	int x, y;

	App->input->GetMousePosition(x, y);

	if (x > screenPos.x&& x < screenPos.x + activeRect.w && y > screenPos.y&& y < screenPos.y + activeRect.h && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		isPressed = true;

		if (isPressed)
		{
			int x_1, y_2;
			App->input->GetMouseMotion(x_1, y_2);
			offset.x = +x_1*2;
			App->audio->ModifyVolume(offset.x);
		}
	}
	//LOG("(%i,%i)", x, y);
}
