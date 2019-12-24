#include "j1UI.h"
#include "UI_Element.h"
#include "p2Log.h"

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
	CreateUIElement({ 0,0,0,0 }, nullptr, { 0,0 }, { 0,0 }, TYPE::UI_NoneType);
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
	for (p2List_item<UI_Element*>* element = UI_Elements_list.start; element != UI_Elements_list.end; ++element) {
		if (!element->data->isHide()) {
			LOG("IM NOT HIDDING");
		}
	}
}

void j1UI::CreateUIElement(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type)
{
	UI_Element* element = new UI_Element(image, text, local, screen, ui_type);
	UI_Elements_list.add(element);
}
