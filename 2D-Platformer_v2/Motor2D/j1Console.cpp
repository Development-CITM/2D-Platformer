#include "j1Console.h"
#include "j1UI.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"
#include "p2Log.h"

j1Console::j1Console()
{
}

j1Console::~j1Console()
{
}

bool j1Console::Start()
{
	input_text = new UI_InputText({ 300,300,20,100 });
	return true;
}

bool j1Console::Update(float dt)
{
	input_text->DrawCursor({ 300,300 }, {2,20});
	LOG("CONSOLE");
	return true;
}

bool j1Console::CleanUp()
{
	return true;
}
