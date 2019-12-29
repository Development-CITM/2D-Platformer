#include "UI_Text.h"
#include "j1App.h"
#include "j1Fonts.h"
#include "j1Textures.h"

UI_Text::UI_Text()
{
}

UI_Text::UI_Text(const char* char_text, p2Point<int> offsetPos, p2Point<int> screen,TextType type, UI_Element* m_parent)
{
	text_type = type;
	parent = m_parent;
	UI_Type = TYPE::UI_Font;
	text = char_text;
	offset = offsetPos;
	screenPos = screen;
	texture = App->fonts->Print(text.GetString());
	
	App->fonts->CalcSize(text.GetString(), length, height);
	int num = length;
	int num1 = height;
}


UI_Text::~UI_Text()
{
}

void UI_Text::UpdateText(const char* textInput)
{
	App->tex->UnLoad(texture);
	text = textInput;
	texture = App->fonts->Print(text.GetString());
}

void UI_Text::CreateText(const char* m_text)
{
	App->tex->UnLoad(texture);
	text += m_text;
	texture = App->fonts->Print(text.GetString());
}
