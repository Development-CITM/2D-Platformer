#ifndef __UI_Image_H__
#define __UI_Image_H__
#include "UI_Element.h"

class UI_Image : public UI_Element
{
public:
	UI_Image(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type,UI_Element* parent);
	virtual ~UI_Image();

};

#endif // !__UI_Image_H__

