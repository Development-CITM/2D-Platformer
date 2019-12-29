#ifndef __UI_INPUT_TEXT_H__
#define __UI_INPUT_TEXT_H__
#include "UI_Element.h"
#include "p2SString.h"
#include "UI_Fonts.h"

class UI_InputText : public UI_Element
{
public:
	UI_InputText();
	~UI_InputText();

	bool hasFocus;

	UI_Fonts* text;
	SDL_Rect rect;

	void DrawCursor(p2Point<int> pos, p2Point<int> size);


};

#endif // !__UI_INPUT_TEXT_H__

