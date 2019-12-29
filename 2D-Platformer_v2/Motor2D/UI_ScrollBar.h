#ifndef __UI_SCROLLBAR_H__
#define __UI_SCROLLBAR_H__
#include "UI_Element.h"

class UI_ScrollBar : public UI_Element
{
public:
	UI_ScrollBar();
	virtual ~UI_ScrollBar();

	SDL_Rect thumb_rect;

	p2Point<int> dragArea;
	p2Point<int> dragPosition;
};

#endif // !__UI_SCROLLBAR_H__

