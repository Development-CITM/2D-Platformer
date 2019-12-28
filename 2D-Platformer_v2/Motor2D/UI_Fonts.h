#ifndef __UI_Fonts_H__
#define __UI_Fonts_H__
#include "UI_Element.h"
#include "SDL\include\SDL_pixels.h"
#include "p2List.h"
#include "p2SString.h"

#define DEFAULT_FONT "fonts/acknowledge/acknowtt.ttf"
#define DEFAULT_FONT_SIZE 30

struct SDL_Texture;
struct _TTF_Font;


class UI_Fonts :public UI_Element {

public:
	UI_Fonts(TYPE type_element, UI_Element* parent_);
	~UI_Fonts();
	void Update();
	bool CleanUp();
	_TTF_Font* const LoadFont(const char* path, int size = 12);
	SDL_Texture* BlitText(const char* text, SDL_Color color , _TTF_Font* font = NULL);
	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;
	void Draw();
	void AddScore(int add);

public:

	p2List<_TTF_Font*>	fonts;
	_TTF_Font* default;
	p2SString score="0";
	int score_int = 0;

	SDL_Texture* text = NULL;
};




#endif
