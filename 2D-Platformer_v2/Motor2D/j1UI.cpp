#include "j1UI.h"
#include "UI_Element.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"

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
	backgroundImage = CreateUIImage({ 67,49,266,510 }, App->tex->Load("UI/HUD_Menus.png"), { 0,0 }, { App->win->GetWidth() - 200,100 }, TYPE::UI_Image);
	playButton = CreateUIButton({ 16,90,234,64 }, { 15,855,234,64 }, { 16,471,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,20 }, { 0,0 }, TYPE::UI_Button, backgroundImage);

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

	UpdateUI();
	LOG("SCREEN POS : (%i,%i)", playButton->GetScreenPos().x, playButton->GetScreenPos().y);
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
		UI_Elements_list[i]->Draw();
	}
}

void j1UI::UpdateUI() 
{
	for (int i = 0; i < UI_Elements_list.count(); i++)
	{
		UI_Elements_list[i]->Update();
	}
}

UI_Element* j1UI::CreateUIImage(SDL_Rect image, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,UI_Element* parent)
{
	UI_Element* element = new UI_Image(image, text, offset , screen, ui_type, parent);
	LOG("Image created");
	UI_Elements_list.add(element);
	return element;
}

UI_Element* j1UI::CreateUIButton(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,UI_Element* parent)
{
	UI_Element* element = new UI_Button(image,hover,pressed, text, offset, screen, ui_type,parent);
	LOG("Button created");
	UI_Elements_list.add(element);
	return element;
}
