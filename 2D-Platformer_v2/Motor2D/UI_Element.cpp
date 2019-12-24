#include "UI_Element.h"
#include "j1Textures.h"
#include "j1App.h"


UI_Element::UI_Element(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type): base_image(image),texture(text),localPos(local),screenPos(screen),UI_Type(ui_type)
{
}

UI_Element::~UI_Element()
{
	texture = nullptr;
}

void UI_Element::SetTexture(const char* path)
{
	texture = App->tex->Load(path);
}

p2Point<int> UI_Element::GetLocalPos()
{
	return localPos;
}

p2Point<int> UI_Element::GetScreenPos()
{
	return screenPos;
}

SDL_Rect UI_Element::GetRect()
{
	return base_image;
}

void UI_Element::ToggleHide(bool flag)
{
	hide = flag;
}

bool UI_Element::isHide()
{
	return hide;
}
