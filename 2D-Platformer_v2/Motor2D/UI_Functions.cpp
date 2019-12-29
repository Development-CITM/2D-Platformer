#include "UI_Functions.h"
#include "p2Log.h"
#include "j1UI.h"
#include "j1App.h"
#include "j1Input.h"
#include "SDL/include/SDL_scancode.h"
#include "j1Scene.h"
#include "j1Debug.h"


UI_Functions::UI_Functions()
{
}

UI_Functions::~UI_Functions()
{
}

void UI_Functions::StartButton(UI_Element* current)
{
		App->scene->destination_level = "maps/A2.tmx";
		App->debug->CallFade();
		current->ToggleHide(true);

}



void UI_Functions::ContinueButton()
{
}

void UI_Functions::SettingsButton(UI_Element* settings, UI_Element* mainmenu)
{
	//Open settings menu
	settings->ToggleHide(false);
	for (int i = 0; i < settings->childs.count(); i++)
	{
		settings->childs[i]->ToggleHide(false);
	}
	mainmenu->ToggleHide(true);
	LOG("OPEN SETTINGS");
}

void UI_Functions::ActiveSoundButton(bool flag)
{
	//Activar desactivar sonido
	//Toggle enable
}

void UI_Functions::CreditsButton(UI_Element* current, UI_Element* target)
{
	current->ToggleHide(true);
	target->ToggleHide(false);
}

void UI_Functions::BackToMainMenu(UI_Element* pause, UI_Element* main_menu)
{
	pause->ToggleHide(true);
	main_menu->ToggleHide(false);
	for (int i = 0; i < main_menu->childs.count(); i++)
	{
		main_menu->childs[i]->ToggleHide(false);
	}
	HideUI(App->ui->coin_background, App->ui->timer_background, App->ui->coin_image, App->ui->timer_image);
	App->scene->destination_level = "maps/A5.tmx";
	App->debug->CallFade();	
}

void UI_Functions::ShowUI(UI_Element* coins, UI_Element*timer,UI_Element* coin_image, UI_Element* timer_image)
{
	coins->ToggleHide(false);
	timer->ToggleHide(false);
	coin_image->ToggleHide(false);
	timer_image->ToggleHide(false);
	coins->childs[0]->ToggleHide(false);
}

void UI_Functions::ShowPauseMenu(UI_Element* pause)
{
	pause->ToggleHide(false);
	for (int i = 0; i < pause->childs.count(); i++)
	{
		pause->childs[i]->ToggleHide(false);
	}
}

void UI_Functions::HideUI(UI_Element* coins, UI_Element* timer, UI_Element* coin_image, UI_Element* timer_image)
{
	coins->ToggleHide(true);
	timer->ToggleHide(true);
	coin_image->ToggleHide(true);
	timer_image->ToggleHide(true);
	coins->childs[0]->ToggleHide(true);
}

void UI_Functions::ReturnButton(UI_Element* current, UI_Element* mainmenu)
{
	//Hide current menu and show main menu
	mainmenu->ToggleHide(false);
	current->ToggleHide(true);
	for (int i = 0; i < mainmenu->childs.count(); i++)
	{
		mainmenu->childs[i]->ToggleHide(false);
	}
	LOG("Close");

}

void UI_Functions::QuitButton()
{
	App->ui->quit = true;
}


