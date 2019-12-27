#include "UI_Image.h"

UI_Image::UI_Image(SDL_Rect image, SDL_Texture* text, p2Point<int> offset, p2Point<int> screen, TYPE ui_type, UI_Element* parent) : UI_Element(image, text, offset, { 0,0 }, screen, ui_type, parent)
{

}

UI_Image::~UI_Image()
{
}
