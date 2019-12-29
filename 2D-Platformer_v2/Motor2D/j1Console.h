#ifndef __j1Console_H__
#define __j1Console_H__
#include "j1Module.h"
#include "UI_Element.h"
#include "UI_InputText.h"

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

};

#endif // !__j1Console_H__
