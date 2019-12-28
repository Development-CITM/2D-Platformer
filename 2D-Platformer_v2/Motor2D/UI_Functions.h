#ifndef __UI_FUNCITONS_H__
#define __UI_FUNCTIONS_H__

class UI_Element;

class UI_Functions
{
	UI_Functions();
	~UI_Functions();

public:
	static void StartButton(UI_Element*);
	static void ContinueButton();
	static void SettingsButton(UI_Element*, UI_Element*);
	static void ActiveSoundButton(bool flag);
	static void ReturnButton(UI_Element*, UI_Element*);
	static void CreditsButton(UI_Element*, UI_Element*);
	static void ShowUI(UI_Element*, UI_Element*, UI_Element*, UI_Element*);
	static void QuitButton();

};


#endif // !__UI_FUNCITONS_H__
