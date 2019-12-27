#include "UI_Element.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1App.h"
#include "j1Render.h"


UI_Element::UI_Element(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type): base_rect(image),texture(text),localPos(local),screenPos(screen),UI_Type(ui_type),hide(false),activeRect(image)
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

SDL_Texture* UI_Element::GetTexture()
{
	return texture;
}

SDL_Rect UI_Element::GetRect()
{
	return base_rect;
}

void UI_Element::Draw()
{	
	App->render->Blit(texture, screenPos.x,screenPos.y, &activeRect);
}

void UI_Element::SetBaseRect(SDL_Rect rect)
{
	base_rect = rect;
}

void UI_Element::ToggleHide(bool flag)
{
	hide = flag;
}

bool UI_Element::isHide()
{
	return hide;
}
