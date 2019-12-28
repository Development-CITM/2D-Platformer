#include "UI_Functions.h"
#include "p2Log.h"
#include "j1UI.h"
#include "j1App.h"

UI_Functions::UI_Functions()
{
}

UI_Functions::~UI_Functions()
{
}

void UI_Functions::StartButton()
{
	LOG("START BUTTON");
}

void UI_Functions::ContinueButton()
{
}

void UI_Functions::SettingsButton(UI_Element* settings, UI_Element* mainmenu)
{
	//Open settings menu
	settings->ToggleHide(false);
	mainmenu->ToggleHide(true);
}

void UI_Functions::ActiveSoundButton(bool flag)
{
	//Activar desactivar sonido
	//Toggle enable
}

void UI_Functions::CreditsButton()
{

}

void UI_Functions::ReturnButton(UI_Element* current, UI_Element* mainmenu)
{
	//Hide current menu and show main menu
	current->ToggleHide(true);
	mainmenu->ToggleHide(false);

}

void UI_Functions::QuitButton()
{
	App->ui->quit = true;
}


