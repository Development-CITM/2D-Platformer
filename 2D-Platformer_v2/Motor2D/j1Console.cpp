#include "j1Console.h"
#include "j1UI.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1Debug.h"
#include "j1UI.h"
#include "UI_Functions.h"
j1Console::j1Console()
{
}

j1Console::~j1Console()
{
}

bool j1Console::Start()
{
	input_text = App->ui->CreateUIInputText({ 600,30 }, { 0,0 }, { 0,400 });
	input_text->ToggleHide(true);

	for (int i = 0; i < App->logs.count(); i++)
	{
			AddLog(App->logs[i].GetString());
		
	}

	for (int i = 0; i < App->modules.count(); i++)
	{
		for (int y = 0; y < App->modules[i]->logs.count(); y++)
		{
			AddLog(App->modules[i]->logs[y].GetString());

		}
	}

	return true;
}

bool j1Console::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN) {
		
		showConsole = !showConsole;
		input_text->ToggleHide(!input_text->isHide());
	}

	if (!showConsole) {
		if (!App->IsPaused)
		{
			App->pause = false;
		}
		return true;
	}


	if (showConsole) {
		App->pause = true;
	}

	int posX = -20;
	for (int i = 0; i < text_list.count(); i++)
	{
		posX += 20;
		text_list[i]->SetScreen({ text_list[i]->GetScreenPos().x,posX });

	}

	p2Point<int> pos = App->render->ScreenToWorld(0, 0);
	App->render->DrawQuad({pos.x,pos.y,600,400 }, 0, 0, 0, 220);

	for (int i = 0; i < text_list.count(); i++)
	{
		text_list[i]->Draw();
	}

	return true;
}

bool j1Console::CleanUp()
{
	return true;
}

void j1Console::AddLog(const char* text)
{
	text_list.add(App->ui->CreateUIText(text, { 0,0 }, { 0,0 }, CONSOLE));
}

void j1Console::ExecuteCommand(p2SString string)
{
	Commands command = none;

	if (string == "quit") {
		command = quit;
	}

	else if (string == "list") {
		command = list;
	}	
	
	else if (string == "map<A2>") {
		command = mapA2;
		
	}
	else if (string == "map<A3>") {
		command = mapA3;
		
	}	
	
	else if (string == "change_fps") {
		command = fps;
		
	}
	else {
		command = error;
	}




	switch (command)
	{
	case quit:
		App->ui->quit = true;
		break;
	case list:
		App->console->AddLog("List option: list, quit, map<A2>, map<A3>, change_fps");
		break;
	case mapA2:
		App->ui->backgroundImage->ToggleHide(true);
		UI_Functions::ShowUI(App->ui->coin_background, App->ui->timer_background, App->ui->coin_image, App->ui->timer_image);
		App->scene->destination_level = "maps/A2.tmx";
		App->debug->CallFade();

		break;
	case mapA3:
		App->ui->backgroundImage->ToggleHide(true);
		UI_Functions::ShowUI(App->ui->coin_background, App->ui->timer_background, App->ui->coin_image, App->ui->timer_image);
		App->scene->destination_level = "maps/A3.tmx";
		App->debug->CallFade();

		break;
	case fps:
		App->maxcapFrames = !App->maxcapFrames;
		App->console->AddLog("Changed fps");
		break;
	case error:
		App->console->AddLog("Error command try another one");
		break;
	}
}
