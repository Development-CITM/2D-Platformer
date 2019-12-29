#include "j1Console.h"
#include "j1UI.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"

j1Console::j1Console()
{
}

j1Console::~j1Console()
{
}

bool j1Console::Start()
{
	//window =App->ui->CreateUIImage({ 11,15,89,41 }, App->tex->Load("UI/coin_timer.png"), { 0,0 }, { App->win->GetWidth() - 930,20 }, TYPE::UI_Image);
	//text_console = App->ui->CreateUIText(window);
	return true;
}

bool j1Console::Update()
{
	input_text->DrawCursor({ 300,300 }, {100,100});
	return true;
}

bool j1Console::CleanUp()
{
	return true;
}
