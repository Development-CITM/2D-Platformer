#ifndef __UI_INPUT_TEXT_H__
#define __UI_INPUT_TEXT_H__
#include "UI_Element.h"
#include "p2SString.h"


class UI_InputText : public UI_Element
{
public:
	UI_InputText(SDL_Rect);
	virtual ~UI_InputText();

	bool hasFocus  = false;

	SDL_Rect cursor_rect;
	SDL_Rect background_rect;


	void Draw() override;
	void DrawCursor(p2Point<int> pos, p2Point<int> size);


};

#endif // !__UI_INPUT_TEXT_H__

