#ifndef __UI_Fonts_H__
#define __UI_Fonts_H__
#include "UI_Element.h"

class UI_Fonts :public UI_Element {

public:
	UI_Fonts(const char* path, const char* characters, int rows, int w, int h, int rc);
	~UI_Fonts();
	bool PreUpdate();
	void Update();
	bool PostUpdate();
	int LoadFont(const char* path, const char* characters, int rows, int w, int h, int rc);
	void BlitText(int x, int y, const char* text, int font_id, int space);


private:
	SDL_Texture* font_texture;
	int font_id;


};




#endif
