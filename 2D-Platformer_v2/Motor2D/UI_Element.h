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
	UI_Font,
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
	bool addScoreCoin=false;

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
	virtual SDL_Rect GetRect();
	virtual void Draw();
	virtual void Update();
	virtual UI_Element* GetUIElement();
	virtual UI_Element* GetUIParent();

	virtual void SetBaseRect(SDL_Rect);
	virtual void ToggleHide(bool flag);
	virtual bool isHide();
};


#endif // !__UI_ELEMENT_H__

