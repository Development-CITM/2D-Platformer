#include "UI_Fonts.h"
#include "UI_Button.h"
#include "j1App.h"
#include "j1Input.h"
#include "SDL/include/SDL_mouse.h"
#include "p2Log.h"
#include "UI_Functions.h"

UI_Fonts::UI_Fonts(const char* path, const char* characters, int rows, int w, int h, int rc) : UI_Element()
{
	font_id = LoadFont(path, characters, rows, w, h, rc);
}

UI_Fonts::~UI_Fonts()
{
}

bool UI_Fonts::PreUpdate()
{
	bool ret = true;
	return ret;
}

void UI_Fonts::Update()
{

}

bool UI_Fonts::PostUpdate()
{
	bool ret = true;
	return ret;
}

int UI_Fonts::LoadFont(const char* path, const char* characters, int rows, int w, int h, int rc)
{
	int id = 0;

	return id;
}
