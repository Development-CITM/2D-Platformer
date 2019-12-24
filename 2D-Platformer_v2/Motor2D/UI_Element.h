#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__
#include "SDL/include/SDL_rect.h"
#include "p2Point.h"


struct SDL_Texture;

enum class TYPE
{
	UI_NoneType,
	UI_Image,
	UI_Button,
	UI_Window,
	UI_Console,
};

class UI_Element
{
private:
	SDL_Rect base_rect;
	SDL_Rect activeRect;
	SDL_Texture* texture;

	p2Point<int> localPos;
	p2Point<int> screenPos;

	TYPE UI_Type;

	bool hide;
	//Public Functions
public:
	UI_Element(SDL_Rect image,SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type);
	~UI_Element();

	virtual void SetTexture(const char* path);

	virtual p2Point<int> GetLocalPos();
	virtual p2Point<int> GetScreenPos();
	virtual SDL_Rect GetRect();
	virtual void SetBaseRect(SDL_Rect);
	virtual void ToggleHide(bool flag);
	virtual bool isHide();
};


#endif // !__UI_ELEMENT_H__

