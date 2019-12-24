#include "UI_Image.h"

UI_Image::UI_Image(SDL_Rect image, SDL_Texture* text, p2Point<int> local, p2Point<int> screen, TYPE ui_type) : UI_Element(image,text,local,screen,ui_type)
{

}

UI_Image::~UI_Image()
{
}
