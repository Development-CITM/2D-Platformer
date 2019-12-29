#ifndef __j1Console_H__
#define __j1Console_H__
#include "j1Module.h"
#include "UI_Element.h"
#include "UI_Fonts.h"
#include "UI_InputText.h"

class j1Console : public j1Module
{
public:
	j1Console();
	virtual ~j1Console();

	bool Start();
	bool Update();
	bool CleanUp();


	UI_Element* window;
	UI_Fonts* text_console;
	UI_InputText* input_text;

};

#endif // !__j1Console_H__
