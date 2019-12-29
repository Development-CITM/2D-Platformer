#ifndef __UI_SCROLLBAR_H__
#define __UI_SCROLLBAR_H__
#include "UI_Element.h"
#include "j1UI.h"
#include "UI_Image.h"

class UI_ScrollBar : public UI_Element
{
public:
	UI_ScrollBar(SDL_Rect image, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type, UI_Element* parent = nullptr);
	virtual ~UI_ScrollBar();

	void Update();


	SDL_Rect thumb_rect;

private:
	bool isPressed = false;
};

#endif // !__UI_SCROLLBAR_H__

