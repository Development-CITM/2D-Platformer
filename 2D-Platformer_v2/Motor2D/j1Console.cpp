#include "j1Console.h"
#include "j1UI.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Input.h"


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


	int pos = -20;
	for (int i = 0; i < text_list.count(); i++)
	{
		pos += 20;
		text_list[i]->SetScreen({ text_list[i]->GetScreenPos().x,pos });

	}

	
	return true;
}

bool j1Console::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN) {
		
		showConsole = !showConsole;
		input_text->ToggleHide(!input_text->isHide());
	}

	if (!showConsole)
		return true;

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
