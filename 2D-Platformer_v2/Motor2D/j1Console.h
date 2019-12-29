#ifndef __j1Console_H__
#define __j1Console_H__
#include "j1Module.h"
#include "UI_Element.h"
#include "UI_InputText.h"
#include "UI_Text.h"

enum Commands {
	none,
	quit,
	list,
	mapA2,
	mapA3,
	fps,
	error,


};

class j1Console : public j1Module
{
public:
	j1Console();
	virtual ~j1Console();

	bool Start();
	bool Update(float dt) override;
	bool CleanUp();


	UI_Element* window;
	UI_InputText* input_text;

	UI_Text* text = nullptr;

	p2List<UI_Text*> text_list;
	p2List<p2SString> log_list;

	bool showConsole = false;
	void AddLog(const char* text);

	void ExecuteCommand(p2SString);
};

#endif // !__j1Console_H__
