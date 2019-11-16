#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Audio.h"
#include "j1Fade2Black.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1Player.h"
#include <math.h>

#pragma region Constructor / Destructor

//Constructor ----------------------------------------------------------------------------------------------------------------------------------
j1Colliders::j1Colliders():j1Module()
{
	name.create("colliders");
}

//Destructor ------------------------------------------------------------------------------------------------------------------------------------
j1Colliders::~j1Colliders()
{}
#pragma endregion


#pragma region Awake/Pre/Update/Post/Clean

//Sets the scale & size for the colliders in function of win scale and render drawsize ----------------------------------------------------------
bool j1Colliders::Awake(pugi::xml_node& conf)
{
	scale = App->win->GetScale();
	size = App->render->drawsize;

	return true;
}

//Removes all colliders scheduled to be deleted -------------------------------------------------------------------------------------------------
bool j1Colliders::PreUpdate()
{
	p2List_item<Collider*>* collider = colliders.start;
	for (uint i = 0; i < colliders.count(); ++i)
	{
		if (collider != nullptr && collider->data->to_delete == true)
		{
			colliders.del(collider);
		}

		collider = collider->next;
	}
	p2List_item<Collider*>* detected_collider = detected_Colliders.start;

	// Disable all colliders in the list ------------------------------------------------------------------
	//for (uint i = 0; i < detected_Colliders.count(); ++i)
	//{
	//	if (App->player->GetPlayerCollider()->rect.y + 30 < detected_collider->data->rect.y) {
	//		detected_collider->data->Enabled = true;
	//		detected_Colliders.del(detected_collider);
	//	}
	//	detected_collider = detected_collider->next;

	//}
	return true;
}

//Draws all colliders ---------------------------------------------------------------------------------------------------------------------------
bool j1Colliders::Update(float dt)
{
	Draw();
	return true;
}

//PostUpdate ------------------------------------------------------------------------------------------------------------------------------------
bool j1Colliders::PostUpdate()
{
	return true;
}

//CleanUp ---------------------------------------------------------------------------------------------------------------------------------------
bool j1Colliders::CleanUp()
{
	//Deletes each collider and clears the list --------------------------------------------------------------
	p2List_item<Collider*>* item2;
	item2 = colliders.start;

	while (item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	colliders.clear();
	return true;
}
#pragma endregion


#pragma region Load Info

// Loads all colliders of the map --------------------------------------------------------------------------------------------------------------
bool j1Colliders::Load(pugi::xml_node& object)
{
	bool ret = true;
		for (object.child("object"); object && ret; object = object.next_sibling("object"))
		{
			//Creates new collider and sends it to be filled --------------------------------------------
			collider = new Collider();
			ret = LoadObject(object, collider);

			//Adds the collider to the colliders list ---------------------------------------------------
			if (ret)
				colliders.add(collider);
		}	
	return ret;
}

// Loads each collider properties -------------------------------------------------------------------------------------------------------------
bool j1Colliders::LoadObject(pugi::xml_node& node, Collider* collider)
{
	bool ret = true;
	//Loads into the collider the values from the tmx --------------------------------------------------
	collider->rect.x = node.attribute("x").as_int();
	collider->rect.y = node.attribute("y").as_int();
	collider->rect.w = node.attribute("width").as_int();
	collider->rect.h = node.attribute("height").as_int();

	//Selects the type of collider ---------------------------------------------------------------------
	if (strcmp(node.attribute("type").as_string(), "Jumpable") == 0)
	{
		collider->type = COLLIDER_WALL_TRASPASSABLE;
	}
	if (strcmp(node.attribute("type").as_string(), "Dead") == 0)
	{
		collider->type = COLLIDER_DEAD;
	}
	if (strcmp(node.attribute("type").as_string(), "Barrier") == 0)
	{
		collider->type = COLLIDER_WALL_SOLID;
	}	
	if (strcmp(node.attribute("type").as_string(), "Exit") == 0)
	{
		collider->type = COLLIDER_EXIT;
		App->scene->exitCollider = collider;
	}
	return ret;
}
#pragma endregion


#pragma region Render

//Draws each collider in the colliders list ---------------------------------------------------------------------------------------------------
void j1Colliders::Draw()
{
	if (collider_debug)
	{
		//Iterates all list ------------------------------------------------------------------------------
		p2List_item<Collider*>* collider = this->colliders.start;
		for (int i = 0; i < colliders.count(); i++)
		{
			//Sets the rect to be printed and it's properties --------------------------------------------
			SDL_Rect rect = { 0,0,0,0 };
			rect.x = collider->data->rect.x * size * scale;
			rect.y = collider->data->rect.y * size * scale;
			rect.w = collider->data->rect.w * size * scale;
			rect.h = collider->data->rect.h * size * scale;
			//Sets the collider type ----------------------------------------------------------------------
			switch (collider->data->type)
			{
			case COLLIDER_WALL_SOLID:
				App->render->DrawQuad(rect, 255, 0, 0, 100);
				break;
			case COLLIDER_WALL_TRASPASSABLE:			
				App->render->DrawQuad(rect, 0, 0, 255, 100);
				break;
			case COLLIDER_DEAD:
				App->render->DrawQuad(rect, 0, 0, 0, 100);
				break;
			case COLLIDER_PLAYER:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				App->render->DrawQuad(rect, 0, 255, 0, 100);
				break;
			case COLLIDER_WINDOW:
				App->render->DrawQuad(rect, 0, 255, 255, 50);
				break;
			case COLLIDER_CEILING_CHECKER:

				App->render->DrawQuad(rect, 0, 0, 0, 255,collider->data->collided);
				break;
			case COLLIDER_EXIT:
				App->render->DrawQuad(rect, 100, 200, 100, 100);
				break;
			}
			//Iterates for next collider in the list ----------------------------------------------------	
			if (collider->next != nullptr)
			{
				collider = collider->next;
			}
		}
	}
}
#pragma endregion

#pragma region AddFunctions

//Adds each collider to the list via creating a new collider and setting it's properties ------------------------------------------------------
Collider* j1Colliders::AddCollider(SDL_Rect rect, ColliderType type,p2Point<int>offset, j1Module* callback, int Damage)
{	
	c = new Collider(rect, type,offset, callback, Damage);
	colliders.add(c);

	return c;
}
#pragma endregion

#pragma region CheckFunctions
//What's the point of this function? Pos Y? --------------------------------------------------------------------------------------------------- EUDALD: ???
bool j1Colliders::CheckColliderCollision(Collider* c1,p2Point<int>increment, int* posX,int* posY)
{	
	bool ret = false;
	Collider prediction;
	prediction = *c1;
	prediction.rect.x = c1->rect.x + increment.x;
	prediction.rect.y = c1->rect.y + increment.y;

	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (int i = 0; i < colliders.count(); i++)
	{

		if (prediction.type != c2->type && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_CEILING_CHECKER)
		{
			if (increment.x != 0) {
				if (prediction.CheckCollision(c2->rect)) {
					if (posX != nullptr && posY != nullptr) {
						*posX = c2->rect.x;
						*posY = c2->rect.y;
					}
					ret = true;
				}
			}
			else if(increment.x == 0){
				if (prediction.CheckCollision(c2->rect)) {
					if (posX != nullptr && posY != nullptr) {
						*posX = c2->rect.x;
						*posY = c2->rect.y;
					}
					ret = true;
				}		
			}
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}

	if (!ret) {
		c1->rect.x = prediction.rect.x;					
		c1->rect.y = prediction.rect.y;
		
	}
	if (ret && increment.y > 0) {
		c1->rect.y = *posY - App->player->player_tmx_data.tile_height - App->player->colliderOffsetY2;
		App->player->jumping = false;	
	}	



	return ret;
}

bool j1Colliders::CheckColliderCollision(Collider* c1,Directions dir, int* snapPos)
{ 
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (int i = 0; i < colliders.count(); i++)
	{
		if (c1->type != c2->type && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_PLAYER)
		{
			if (c1->CheckCollision(c2->rect)) {
				if (dir != Directions::DIR_NONE && snapPos != nullptr) {
					switch (dir)
					{
					case Directions::DIR_RIGHT:
						if (c2->type != COLLIDER_WALL_TRASPASSABLE) {
							*snapPos = c2->rect.x - 22;
						}
						else {
							*snapPos = 0;
						}
						break;

					case Directions::DIR_LEFT:
						if (c2->type != COLLIDER_WALL_TRASPASSABLE) {
							*snapPos = c2->rect.x + c2->rect.w + 2;
						}
						else {
							*snapPos = 0;
						}
						break;
					case Directions::DIR_DOWN:
						*snapPos = c2->rect.y -46;
						break;
					case Directions::DIR_UP:
						//*snapPos = c2->rect.y -46;
						break;
					}

				}
				ret = true;
			}
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}
	return ret;
}

bool j1Colliders::ThroughPlatform(Collider* c1)
{
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (uint i = 0; i < colliders.count(); i++)
	{
		if (c1 != c2 && c2->type != COLLIDER_WINDOW) {

			if (c2->type == COLLIDER_WALL_TRASPASSABLE) {
				if (c1->CheckCollision(c2->rect)) {
						ret = false;
						c2->Enabled = false;
						if (detected_Colliders.find(c2) == -1)
							detected_Colliders.add(c2);				
				}
			}
			else 
				ret = c1->CheckCollision(c2->rect);
			
				return ret;
			
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}

	return ret;
}

//Checks whenever the player collider is in collision with a map collider -----------------------------------------------------------------
bool Collider::CheckCollision(const SDL_Rect& r) const
{

	bool detectedX = true;
	bool detectedY = true;

	if ((rect.x + rect.w) < r.x || (r.x + r.w) < rect.x) {
		detectedX = false;
	}

	if ((rect.y + rect.h) < r.y || (r.y + r.h) < rect.y)
	{
		detectedY = false;
	}

	return detectedX && detectedY;

}
#pragma endregion