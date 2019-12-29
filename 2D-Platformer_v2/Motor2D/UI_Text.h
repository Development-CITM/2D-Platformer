#ifndef __UI_TEXT_H__
#define __UI_TEXT_H__
#include "UI_Element.h"
#include "j1Fonts.h"
#include "p2Point.h"
#include "j1Textures.h"

class UI_Text : public UI_Element
{
public:
	UI_Text();
	UI_Text(const char*,p2Point<int> offsetPos, p2Point<int> screenPos,UI_Element*);
	virtual ~UI_Text();

	p2SString text;

	void UpdateText(const char*);
};

#endif // !__UI_TEXT_H__
