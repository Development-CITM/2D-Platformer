#include "UI_InputText.h"
#include "j1Render.h"
#include "j1App.h"


UI_InputText::UI_InputText(SDL_Rect rect)
{
	background_rect = rect;
}

UI_InputText::~UI_InputText()
{
}

void UI_InputText::Draw()
{
	p2Point<int> pos;
	pos.x = App->render->ScreenToWorld(background_rect.x, background_rect.y).x;
	pos.y = App->render->ScreenToWorld(background_rect.x, background_rect.y).y;

	App->render->DrawQuad({ pos.x,pos.y,background_rect.w,background_rect.h }, 255, 255, 255);
}

void UI_InputText::DrawCursor(p2Point<int> pos, p2Point<int> size)
{
	App->render->DrawQuad({ App->render->ScreenToWorld(pos.x,pos.y).x,App->render->ScreenToWorld(pos.x,pos.y).y,size.x,size.y },255,255,255);
}
	