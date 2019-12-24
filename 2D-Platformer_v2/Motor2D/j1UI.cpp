#include "j1UI.h"
#include "UI_Element.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"

j1UI::j1UI()
{
}

j1UI::~j1UI()
{
	for (p2List_item<UI_Element*>* ui_element = UI_Elements_list.start; ui_element != UI_Elements_list.end; ui_element++)
	{
		
	}
}

bool j1UI::Awake(pugi::xml_node&)
{
	bool ret = true;
	return ret;

}

bool j1UI::Start()
{
	bool ret = true;
	CreateUIButton({ 0,0,0,0 }, { 0,0,0,0 }, { 0,0,0,0 }, nullptr, { 0,0 }, { 0,0 }, TYPE::UI_Button);
	CreateUIImage({ 0,0,0,0 },nullptr, { 0,0 }, { 0,0 }, TYPE::UI_Image);
	return ret;
}

bool j1UI::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool j1UI::Update(float dt)
{
	bool ret = true;

	Draw();

	return ret;
}

bool j1UI::PostUpdate()
{
	bool ret = true;
	return ret;
}

void j1UI::Draw()
{
	for (int i = 0; i < UI_Elements_list.count(); i++)
	{
		LOG("%i",UI_Elements_list[i]->UI_Type);
	
	}
}

void j1UI::CreateUIImage(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type)
{
	UI_Element* element = new UI_Image(image, text, local, screen, ui_type);
	LOG("Image created");
	UI_Elements_list.add(element);
}

void j1UI::CreateUIButton(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type)
{
	UI_Element* element = new UI_Button(image,hover,pressed, text, local, screen, ui_type);
	LOG("Button created");
	UI_Elements_list.add(element);
}
