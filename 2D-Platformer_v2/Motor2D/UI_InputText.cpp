#include "UI_InputText.h"
#include "j1Render.h"
#include "j1App.h"

UI_InputText::UI_InputText()
{

}

UI_InputText::~UI_InputText()
{
}

void UI_InputText::DrawCursor(p2Point<int> pos, p2Point<int> size)
{

	App->render->DrawQuad({ pos.x,pos.y,size.x,size.y },255,255,255,255,true,false);
}
	