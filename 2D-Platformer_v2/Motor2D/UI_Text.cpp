#include "UI_Text.h"
#include "j1App.h"
#include "j1Fonts.h"

UI_Text::UI_Text()
{
}

UI_Text::UI_Text(const char* char_text, p2Point<int> offsetPos, p2Point<int> screen, UI_Element* m_parent)
{
	parent = m_parent;
	UI_Type = TYPE::UI_Font;
	text = char_text;
	offset = offsetPos;
	screenPos = screen;
	texture = App->fonts->Print(text.GetString());
}


UI_Text::~UI_Text()
{
}

void UI_Text::UpdateText(const char* textInput)
{
	text = textInput;
	texture = App->fonts->Print(text.GetString());
}
