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

void UI_Functions::SettingsButton()
{
}

void UI_Functions::CreditsButton()
{
}

void UI_Functions::QuitButton()
{
	App->ui->quit = true;
}


