#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__
#include "SDL/include/SDL_rect.h"
#include "p2Point.h"
#include "p2List.h"

struct SDL_Texture;

enum class TYPE
{
	UI_NoneType,
	UI_Image,
	UI_Button,
	UI_Window,
	UI_Console,
	UI_Font,
};

enum class font_type {
	FONT_SCORE,
	FONT_TIMER,
	FONT_CONSOLE
};

class UI_Element
{
	friend class j1UI;
protected:
	SDL_Rect base_rect;
	SDL_Rect activeRect;
	SDL_Texture* texture;
	UI_Element* parent = nullptr;

	p2Point<int> localPos;
	p2Point<int> screenPos;
	p2Point<int> offset;


	TYPE UI_Type;

	bool hide;
	//Public Functions
public:
	UI_Element(SDL_Rect image,SDL_Texture* text, p2Point<int> offset,p2Point<int> local, p2Point<int> screen, TYPE ui_type,UI_Element* parent = nullptr);
	UI_Element();
	~UI_Element();

	virtual void SetTexture(const char* path);

	virtual p2Point<int> GetLocalPos();
	virtual p2Point<int> GetScreenPos();
	virtual SDL_Texture* GetTexture();
	virtual void SetScreen(p2Point<int> newScreen);
	virtual SDL_Rect GetRect();
	virtual void Draw();
	virtual void Update();
	virtual UI_Element* GetUIElement();
	virtual UI_Element* GetUIParent();
	virtual void SetUIParent(UI_Element* parent);
	virtual void SetUIType(TYPE type);

	virtual void SetBaseRect(SDL_Rect);
	virtual void ToggleHide(bool flag);
	virtual bool isHide();
	p2List<UI_Element*> childs;
};


#endif // !__UI_ELEMENT_H__

