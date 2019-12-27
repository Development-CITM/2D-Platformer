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
#include "j1Debug.h"
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
	//p2List_item<Collider*>* collider = colliders.start;
	//for (uint i = 0; i < colliders.count(); ++i)
	//{
	//	if (collider != nullptr && collider->data->to_delete == true)
	//	{
	//		colliders.del(collider);
	//	}
	//	if (collider != nullptr) {
	//		collider = collider->next;
	//	}
	//}
	p2List_item<Collider*>* detected_collider = detected_Colliders.start;

	 //Disable all colliders in the list ------------------------------------------------------------------
	for (uint i = 0; i < detected_Colliders.count(); ++i)
	{
		if (App->entity->RetreivePlayerCollider()->rect.y + 45 < detected_collider->data->rect.y) {
			detected_collider->data->Enabled = true;
			detected_Colliders.del(detected_collider);
		}
		detected_collider = detected_collider->next;

	}
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
	if (strcmp(node.attribute("type").as_string(), "Water") == 0)
	{
		collider->type = COLLIDER_WATER;
	}
	if (strcmp(node.attribute("type").as_string(), "Transition") == 0)
	{
		collider->type = COLLIDER_TRANSITION;	
		pugi::xml_node swap = node.child("properties").child("property");
		ChooseSwap(swap, collider);
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
			if (collider->data->to_delete) {
				collider->data->Enabled = false;
			}
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
				App->render->DrawQuad(rect, 255, 100, 0, 100);
				break;
			case COLLIDER_TRANSITION:
			{
				App->render->DrawQuad(rect, 182, 149, 192,100);
				break;
			}
			case COLLIDER_WALL_TRASPASSABLE:			
				App->render->DrawQuad(rect, 0, 0, 255, 100);
				break;
			case COLLIDER_DEAD:
				App->render->DrawQuad(rect, 0, 0, 0, 200);
				break;
			case COLLIDER_WATER:
				App->render->DrawQuad(rect, 0, 183, 235, 200);	
				break;
			case COLLIDER_PLAYER:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				App->render->DrawQuad(rect, 0, 255, 0, 100);
				break;
			case COLLIDER_PLAYER_HIT:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				if(collider->data->Enabled)
				App->render->DrawQuad(rect, 255, 0, 0, 60);
				break;
			case COLLIDER_ENEMY:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				if(!collider->data->to_delete)
				App->render->DrawQuad(rect, 255, 0, 0, 60);
				break;	
			case COLLIDER_ENEMY_HIT:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				if(!collider->data->to_delete && collider->data->Enabled)
				App->render->DrawQuad(rect, 255, 0, 0, 60);
				break;
			case COLLIDER_COIN:
				rect.x = collider->data->rect.x * size * scale;
				rect.y = collider->data->rect.y * size * scale;
				rect.w = collider->data->rect.w * size * scale;
				rect.h = collider->data->rect.h * size * scale;
				if (collider->data->Enabled)
				App->render->DrawQuad(rect, 128, 0, 128, 60);
				break;
			case COLLIDER_WINDOW:
				App->render->DrawQuad(rect, 0, 255, 255, 50);
				break;
			case COLLIDER_CEILING_CHECKER:
				if (!collider->data->to_delete)
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


bool j1Colliders::CheckColliderCollision(Collider* c1,Directions_v2 dir, int* snapPos)
{ 
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (int i = 0; i < colliders.count(); i++)
	{
		if (c1->type != c2->type && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_PLAYER && c2->type != COLLIDER_PLAYER_HIT && c2->type != COLLIDER_ENEMY && c2->type != COLLIDER_DEAD)
		{
			if (c1->CheckCollision(c2->rect) && c2->Enabled) {
				if (dir != Directions_v2::DIR_NONE_v2 && snapPos != nullptr) {
					switch (dir)
					{
					case Directions_v2::DIR_RIGHT_v2:
							*snapPos = c2->rect.x - 22;						
						ret = true;
						break;

					case Directions_v2::DIR_LEFT_v2:
							*snapPos = c2->rect.x + c2->rect.w + 2;

						ret = true;
						break;
					case Directions_v2::DIR_DOWN_v2:
						*snapPos = c2->rect.y - App->entity->RetreivePlayerCollider()->rect.h - 2;
						ret = true;
						break;
					case Directions_v2::DIR_UP_v2:
						if (c2->type != COLLIDER_WALL_TRASPASSABLE) {
							*snapPos = c2->rect.y + c2->rect.h + 5;
							ret = true;
						}
						else {
							ret = false;
						}
						//*snapPos = c2->rect.y -46;
						break;
					}
				}
			}
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}
	return ret;
}
bool j1Colliders::CheckColliderCollision(Collider* c1)
{
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (int i = 0; i < colliders.count(); i++)
	{
		if (c1->type != c2->type && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_PLAYER && c2->type != COLLIDER_DEAD && c2->type != COLLIDER_ENEMY)
		{
			if (c1->CheckCollision(c2->rect)) {
				if (c1->checkerType == ColliderChecker::Top && c2->type == COLLIDER_WALL_TRASPASSABLE && c2->Enabled) {
					c2->Enabled = false;
					detected_Colliders.add(c2);
				}
				if ((c1->checkerType == ColliderChecker::Right || c1->checkerType == ColliderChecker::Left || c1->checkerType == ColliderChecker::Ground || c1->checkerType == ColliderChecker::Top) && c2->type == COLLIDER_TRANSITION && c2->Enabled) {
					//c2->Enabled = false;
				App->scene->ColliderMapToLoad(c2); //EUDALD: change site
				}
				if (c1->checkerType == ColliderChecker::Ground && c2->type == COLLIDER_WATER && c2->Enabled) {
					App->audio->PlayFx(3);
					c2->Enabled = false;
					App->debug->CallFade();
				}
				if ((c1->checkerType == ColliderChecker::Right || c1->checkerType == ColliderChecker::Left || c1->checkerType == ColliderChecker::Ground || c1->checkerType == ColliderChecker::Top) && c2->type == COLLIDER_COIN && c2->Enabled) {
					c2->Enabled = false;
					App->audio->PlayFx(9);
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

bool j1Colliders::CheckColliderCollision(Collider* c1 , ColliderType type)
{
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (int i = 0; i < colliders.count(); i++)
	{
		switch (type)
		{
		case COLLIDER_DEAD:
			if (c1->CheckCollision(c2->rect) && c2->type == type && c2->Enabled) {
				ret = true;
			}
			break;
		case COLLIDER_COIN:
			if (c1->CheckCollision(c2->rect) && c2->type == type && c2->Enabled) {
				ret = true;
			}
			break;
		case COLLIDER_ENEMY:
			if (c1->CheckCollision(c2->rect) && c2->type == type && c2->Enabled) {
				ret = true;
			}
			break;	
		case COLLIDER_ENEMY_HIT:
			if (c1->CheckCollision(c2->rect) && c2->type == type && c2->Enabled) {
				ret = true;
			}
			break;

		case COLLIDER_PLAYER_HIT:
			if (c1->CheckCollision(c2->rect) && c2->type == type && c2->Enabled) {
				ret = true;
			}
			break;
		case COLLIDER_CEILING_CHECKER:
			if (c1->CheckCollision(c2->rect) && c2->type != COLLIDER_ENEMY && c2->type != COLLIDER_ENEMY_HIT && c2->type != COLLIDER_CEILING_CHECKER && c2->type != COLLIDER_WINDOW && c2->Enabled) {
				ret = true;
			}
			break;
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}
	return ret;
}

void j1Colliders::ChooseSwap(pugi::xml_node& node,Collider* c)
{
	if (strcmp(node.attribute("value").as_string(), "A2toA1")==0)
	{
		c->swap = SwapTo::A2_TO_A1;
	}
	else if (strcmp(node.attribute("value").as_string(), "A1toA2") == 0)
	{
		c->swap = SwapTo::A1_TO_A2;
	}
	else if (strcmp(node.attribute("value").as_string(), "A3toA2") == 0)
	{
		c->swap = SwapTo::A3_TO_A2;
	}
	else if (strcmp(node.attribute("value").as_string(), "A2toA3") == 0)
	{
		c->swap = SwapTo::A2_TO_A3;
	}
	else if (strcmp(node.attribute("value").as_string(), "A1toA5") == 0)
	{
		c->swap = SwapTo::A1_TO_A5;
	}
	else if (strcmp(node.attribute("value").as_string(), "A5toA1") == 0)
	{
		c->swap = SwapTo::A5_TO_A1;
	}
	else if (strcmp(node.attribute("value").as_string(), "A5toA6") == 0)
	{
		c->swap = SwapTo::A5_TO_A6;
	}
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