#ifndef __UI_INPUT_TEXT_H__
#define __UI_INPUT_TEXT_H__
#include "UI_Element.h"
#include "p2SString.h"

class UI_InputText : public UI_Element
{
public:
	UI_InputText();
	~UI_InputText();

	p2SString label;

};

#endif // !__UI_INPUT_TEXT_H__

