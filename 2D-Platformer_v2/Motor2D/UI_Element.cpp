#include "UI_Element.h"
#include "j1Textures.h"
#include "j1App.h"
#include "j1App.h"
#include "j1Render.h"
#include "UI_Button.h"
#include "j1Scene.h"
#include "UI_Text.h"
#include "j1Console.h"

class UI_Button;
UI_Element::UI_Element(SDL_Rect image, SDL_Texture* text, p2Point<int> offset,p2Point<int> local, p2Point<int> screen, TYPE ui_type, UI_Element* parent_base): base_rect(image),texture(text),offset(offset),localPos(local),screenPos(screen),UI_Type(ui_type),hide(false),activeRect(image),parent(parent_base)
{
}

UI_Element::UI_Element()
{
}

UI_Element::~UI_Element()
{
	texture = nullptr;
}

void UI_Element::SetTexture(const char* path)
{
	texture = App->tex->Load(path);
}

p2Point<int> UI_Element::GetLocalPos()
{
	return localPos;
}

p2Point<int> UI_Element::GetScreenPos()
{
	
	return screenPos;
}

SDL_Texture* UI_Element::GetTexture()
{
	return texture;
}

void UI_Element::SetScreen(p2Point<int> newScreen)
{
	screenPos = newScreen;
}

SDL_Rect UI_Element::GetRect()
{
	return base_rect;
}

void UI_Element::Draw()
{	
	if (hide) {
		UI_Button* button = (UI_Button*)this;
		button->isEnabled = false;
		return;
	}
	else {
		UI_Button* button = (UI_Button*)this;
		button->isEnabled = true;
	}

	if (parent != nullptr) {
		if (parent->isHide()) {
			ToggleHide(true);
			return;
		}
	}

	if (!App->scene->saving) {
		UI_Button* button = (UI_Button*)this;
		if (button->button_type == ButtonType::Continue) {
			button->isEnabled = false;
		}
	}

	if (UI_Type == TYPE::UI_Console) {
		return;
	}
	if (UI_Type == TYPE::UI_Font) {
		UI_Text* element = (UI_Text*)this;
		if (element->text_type == CONSOLE && !App->console->showConsole) {
			return;
		}
		localPos = App->render->ScreenToWorld(screenPos.x, screenPos.y);
		App->render->Blit(texture, localPos.x - activeRect.w / 2 + offset.x, localPos.y + offset.y);
	}
	



	if (parent == nullptr) {

		localPos = App->render->ScreenToWorld(screenPos.x, screenPos.y);
		App->render->Blit(texture, localPos.x - activeRect.w / 2 + offset.x, localPos.y + offset.y, &activeRect);
	}
	else {
		localPos = parent->GetLocalPos();
		screenPos.x = parent->GetScreenPos().x - activeRect.w / 2 + offset.x;
		screenPos.y = parent->GetScreenPos().y + offset.y;
		App->render->Blit(texture, localPos.x - activeRect.w / 2 + offset.x, localPos.y + offset.y, &activeRect);
	}
		
	
}

void UI_Element::Update()
{

}

UI_Element* UI_Element::GetUIElement()
{
	return this;
}

UI_Element* UI_Element::GetUIParent()
{
	return parent;
}

void UI_Element::SetUIParent(UI_Element* m_parent)
{
	parent = m_parent;
}

void UI_Element::SetUIType(TYPE type)
{
	UI_Type = type;
}

void UI_Element::SetBaseRect(SDL_Rect rect)
{
	base_rect = rect;
}

void UI_Element::ToggleHide(bool flag)
{
	hide = flag;
}

bool UI_Element::isHide()
{
	return hide;
}
