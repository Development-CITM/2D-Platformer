#include "UI_Fonts.h"
#include "UI_Button.h"
#include "j1App.h"
#include "j1Input.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"

#include "SDL\include\SDL.h"
#include "SDL_TTF\include\SDL_ttf.h"
#pragma comment( lib, "SDL_ttf/libx86/SDL2_ttf.lib" )

UI_Fonts::UI_Fonts(TYPE type_element, UI_Element* parent_) : UI_Element()
{
	UI_Type = type_element;
	parent = parent_;
	if (TTF_Init() == -1)
	{
		LOG("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		const char* path = DEFAULT_FONT;
		int size = DEFAULT_FONT_SIZE;
		default = LoadFont(path, size);
	}
	score = "0";
	texture = BlitText(score.GetString(), { (0,0,0,0) }, default);
}

UI_Fonts::~UI_Fonts()
{}




void UI_Fonts::Update()
{
	if (addScoreCoin)
	{
		AddScore(1);
		addScoreCoin = false;
	}
}

bool UI_Fonts::CleanUp()
{
	LOG("Freeing True Type fonts and library");
	p2List_item<TTF_Font*>* item;

	for (item = fonts.start; item != NULL; item = item->next)
	{
		TTF_CloseFont(item->data);
	}

	fonts.clear();
	TTF_Quit();
	return true;
}

_TTF_Font* const UI_Fonts::LoadFont(const char* path, int size)
{
	TTF_Font* font = TTF_OpenFont(path, size);

	if (font == NULL)
	{
		LOG("Could not load TTF font with path: %s. TTF_OpenFont: %s", path, TTF_GetError());
	}
	else
	{
		LOG("Successfully loaded font %s size %d", path, size);
		fonts.add(font);
	}

	return font;
}

SDL_Texture* UI_Fonts::BlitText(const char* text, SDL_Color color, _TTF_Font* font)
{
	SDL_Texture* ret = NULL;
	SDL_Surface* surface = TTF_RenderText_Blended((font) ? font : default, text, color);

	if (surface == NULL)
	{
		LOG("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		ret = App->tex->LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return ret;
}

bool UI_Fonts::CalcSize(const char* text, int& width, int& height, _TTF_Font* font) const
{
	bool ret = false;

	if (TTF_SizeText((font) ? font : default, text, & width, & height) != 0)
		LOG("Unable to calc size of text surface! SDL_ttf Error: %s\n", TTF_GetError());
	else
		ret = true;

	return ret;
}

void UI_Fonts::Draw()
{
	if (parent == nullptr)
	{
		localPos = App->render->ScreenToWorld(75, 23);
		App->render->Blit(texture, localPos.x, localPos.y);
	}
	else
	{
		localPos = parent->GetLocalPos();
		screenPos.x = parent->GetScreenPos().x - activeRect.w / 2 + offset.x;
		screenPos.y = parent->GetScreenPos().y + offset.y;
		App->render->Blit(texture, localPos.x - 17, localPos.y+7);
	}
}

void UI_Fonts::AddScore(int add)
{
	score_int += add;
	p2SString score = p2SString("%d", score_int);

	texture = BlitText(score.GetString(), { (0,0,0,0) }, default);
}


