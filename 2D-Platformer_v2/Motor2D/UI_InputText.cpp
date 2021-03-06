#include "UI_InputText.h"
#include "j1Render.h"
#include "j1App.h"
#include "j1Input.h"
#include "p2Log.h"
#include "j1UI.h"
#include "j1Console.h"
UI_InputText::UI_InputText(SDL_Rect rect, const char* text)
{

	input_text = App->ui->CreateUIText(text, offset, screenPos);
	input_text->SetUIType(TYPE::UI_Console);
	background_rect = rect;
	activeRect = background_rect;
}

UI_InputText::~UI_InputText()
{
}

void UI_InputText::Update()
{
	int x, y;

	App->input->GetMousePosition(x, y);
	if (x > screenPos.x&& x < screenPos.x + activeRect.w && y > screenPos.y&& y < screenPos.y + activeRect.h && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && !hasFocus) {
		hasFocus = true;
		SDL_StartTextInput();
		input_text->UpdateText("");
		input_text->UpdateText(input_text->text.GetString());
		LOG("FOCUSED");
	}

	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && hasFocus) {
		App->console->AddLog(input_text->text.GetString());
		App->console->ExecuteCommand(input_text->text.GetString());
		input_text->text.Clear();
		input_text->UpdateText(input_text->text.GetString());
	}
}

void UI_InputText::Draw()
{
	if (hide)
		return;
	

	if (parent == nullptr) {

		localPos = App->render->ScreenToWorld(screenPos.x, screenPos.y);
		App->render->DrawQuad({ localPos.x + offset.x,localPos.y + offset.y,background_rect.w,background_rect.h }, 0, 0, 255, 200);
		if (hasFocus) {
			DrawCursor({ localPos.x + input_text->length ,localPos.y }, { 2,30 });
			if(input_text->GetTexture() != nullptr)
			App->render->Blit(input_text->GetTexture(), localPos.x, localPos.y);
		}
		if(!hasFocus && input_text->GetTexture() != nullptr)
		App->render->Blit(input_text->GetTexture(), localPos.x, localPos.y);
	}
	else {
		localPos = parent->GetLocalPos();
		screenPos.x = parent->GetScreenPos().x - activeRect.w / 2 + offset.x;
		screenPos.y = parent->GetScreenPos().y + offset.y;
		App->render->DrawQuad({ localPos.x + offset.x,localPos.y + offset.y,background_rect.w,background_rect.h }, 0, 0, 255, 200);
		if (hasFocus) {
			DrawCursor({ localPos.x + input_text->length ,localPos.y }, { 2,30 });
		
		}

		if(!hasFocus && input_text->GetTexture() != nullptr)
		App->render->Blit(input_text->GetTexture(), localPos.x, localPos.y);
	}


}

void UI_InputText::DrawCursor(p2Point<int> pos, p2Point<int> size)
{
	App->render->DrawQuad({localPos.x,localPos.y,size.x,size.y },255,255,255);
}
	