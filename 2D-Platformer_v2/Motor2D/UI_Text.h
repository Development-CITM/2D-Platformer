#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__
#include "UI_Element.h"
#include "j1Fonts.h"
#include "p2Point.h"
#include "j1Textures.h"

enum TextType {
	HUD,
	CONSOLE,
};

class UI_Text : public UI_Element
{
public:
	UI_Text();
	UI_Text(const char*,p2Point<int> offsetPos, p2Point<int> screenPos,TextType type = HUD,UI_Element* = nullptr);
	virtual ~UI_Text();

	int length = 0;
	int height = 0;
	p2SString text;
	TextType text_type;

	void UpdateText(const char*);
	void CreateText(const char* text);
};

#endif // !__UI_TEXT_H__
