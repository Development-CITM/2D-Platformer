#ifndef __j1Console_H__
#define __j1Console_H__
#include "j1Module.h"
#include "UI_Element.h"
#include "UI_InputText.h"
#include "UI_Text.h"

class j1Console : public j1Module
{
public:
	j1Console();
	virtual ~j1Console();

	bool Start();
	bool Update(float dt) override;
	bool CleanUp();


	UI_Element* window;
	UI_Element* input_text;

	UI_Text* text = nullptr;

	p2List<UI_Text*> text_list;
	p2List<p2SString> log_list;
};

#endif // !__j1Console_H__
