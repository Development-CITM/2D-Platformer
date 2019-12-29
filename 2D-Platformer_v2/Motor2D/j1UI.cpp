#include "j1UI.h"
#include "UI_Element.h"
#include "p2Log.h"
#include "UI_Button.h"
#include "UI_Image.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "UI_Functions.h"
#include "PlayerObjects.h"
#include "j1Fonts.h"


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
	//HUD
	coin_background = CreateUIImage({ 11,15,89,41 }, App->tex->Load("UI/coin_timer.png"), { 0,0 }, { App->win->GetWidth() - 930,20 }, TYPE::UI_Image);
	coin_background->ToggleHide(true);
	timer_background = CreateUIImage({ 11,15,89,41 }, App->tex->Load("UI/coin_timer.png"), { 0,0 }, { App->win->GetWidth() - 110,20 }, TYPE::UI_Image);
	timer_background->ToggleHide(true);
	coin_image = CreateUIImage({ 0,0,52,55 }, App->tex->Load("UI/coin.png"), { -70,0 }, { App->win->GetWidth() - 976,12 }, TYPE::UI_Image, coin_background);
	timer_image = CreateUIImage({ 0,0,52,55 }, App->tex->Load("UI/timer.png"), { -70,0 }, { App->win->GetWidth() - 156,13 }, TYPE::UI_Image, timer_background);

	//Menu
	bool ret = true;
	backgroundImage = CreateUIImage({ 67,49,266,510 }, App->tex->Load("UI/HUD_Menus.png"), { 0,0 }, { App->win->GetWidth() - 200,100 }, TYPE::UI_Image);
	
	playButton = (UI_Button*)CreateUIButton({ 16,90,234,64 }, { 15,855,234,64 }, { 16,471,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,40 }, { 0,0 }, TYPE::UI_Button, ButtonType::Start, backgroundImage);

	continueButton = (UI_Button*)CreateUIButton({ 16,167,234,64 }, { 15,932,234,64 }, { 16,548,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,123 }, { 0,0 }, TYPE::UI_Button, ButtonType::Continue, backgroundImage);
	continueButton->isEnabled = false;
	
	settingsButton = (UI_Button*)CreateUIButton({ 16,317,234,64 }, { 15,1082,234,64 }, { 16,698,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,206 }, { 0,0 }, TYPE::UI_Button, ButtonType::Settings, backgroundImage);

	creditsButton = (UI_Button*)CreateUIButton({ 16,18,234,64 }, { 15,783,234,64 }, { 16,399,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,289 }, { 0,0 }, TYPE::UI_Button, ButtonType::Credits, backgroundImage);

	quitButton = (UI_Button*)CreateUIButton({ 16,243,234,64 }, { 15,1008,234,64 }, { 16,624,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,372 }, { 0,0 }, TYPE::UI_Button, ButtonType::Quit, backgroundImage);

	backgroundImage->childs.add(playButton);
	backgroundImage->childs.add(continueButton);
	backgroundImage->childs.add(settingsButton);
	backgroundImage->childs.add(creditsButton);
	backgroundImage->childs.add(quitButton);


	//Settings
	//Background
	settingsBackground = CreateUIImage({ 3,3,335,192 }, App->tex->Load("UI/SettingsBackground.png"), { 0,0 }, { App->win->GetWidth() - 200,250 }, TYPE::UI_Image);
	settingsBackground->ToggleHide(true);
	//Buttons
	soundButton = (UI_Button*)CreateUIButton({ 498,18,64,63 }, { 498,783,64,63 }, { 498,399,64,63 }, App->tex->Load("UI/Buttons.png"), { -50,20 }, { 0,0 }, TYPE::UI_Button, ButtonType::Sound, settingsBackground);	

	muteButton = (UI_Button*)CreateUIButton({ 581,18,64,63 }, { 581,783,64,63 }, { 581,399,64,63 }, App->tex->Load("UI/Buttons.png"), { 50,20 }, { 0,0 }, TYPE::UI_Button, ButtonType::Mute, settingsBackground);

	returnButton = (UI_Button*)CreateUIButton({ 411,178,234,64 }, { 411,943,234,64 }, { 411,559,234,64 }, App->tex->Load("UI/Buttons.png"), { 0,100 }, { 0,0 }, TYPE::UI_Button, ButtonType::Return, settingsBackground);

	settingsBackground->childs.add(soundButton);
	settingsBackground->childs.add(muteButton);
	settingsBackground->childs.add(returnButton);

	//Credits
	creditsImage = CreateUIImage({ 0,0,862,534 }, App->tex->Load("UI/HUD_Menus Alt2.png"), { 0,0 }, { App->win->GetWidth()/2, 100 }, TYPE::UI_Image);
	creditsImage->ToggleHide(true);

	//Score
	score_text = CreateUIText("0", { -5,4 }, {100,100},coin_background);
	coin_background->childs.add(score_text);
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

	if (quit) {
		ret = false;
	}
	Draw();
	UpdateUI();

	
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
	for (int i = 0; i < App->entity->objects.count(); i++)
	{
		if (App->entity->objects[i]->type_object == Object_type::PLAYER) {
			p2SString text = p2SString("%d",App->entity->objects[i]->score);
			score_text->UpdateText(text.GetString());
		}
	}

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

UI_Element* j1UI::CreateUIButton(SDL_Rect image, SDL_Rect hover, SDL_Rect pressed, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type,ButtonType button, UI_Element* parent)
{
	UI_Element* element = new UI_Button(image,hover,pressed, text, offset, screen, ui_type,button,parent);
	LOG("Button created");
	UI_Elements_list.add(element);
	return element;
}

UI_Text* j1UI::CreateUIText(const char* text,p2Point<int> offset, p2Point<int> screen, UI_Element* parent)
{
	UI_Text* element = new UI_Text(text,offset,screen,parent);
	UI_Elements_list.add(element);
	return element;
}


