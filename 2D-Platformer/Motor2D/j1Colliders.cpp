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

j1Colliders::j1Colliders():j1Module()
{
	name.create("colliders");
}

j1Colliders::~j1Colliders()
{}

#pragma endregion

#pragma region Load Info




bool j1Colliders::Load(pugi::xml_node object)
{
	
	bool ret = true;
	
		for (object.child("object"); object && ret; object = object.next_sibling("object"))
		{
			collider = new Collider();

			ret = LoadObject(object, collider);

			if (ret == true)
			{

				colliders.add(collider);
			}
		}
	
	return ret;
}



bool j1Colliders::LoadObject(pugi::xml_node& node, Collider* collider)
{
	bool ret = true;
	collider->rect.x = node.attribute("x").as_int();
	collider->rect.y = node.attribute("y").as_int();
	collider->rect.w = node.attribute("width").as_int();
	collider->rect.h = node.attribute("height").as_int();

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

#pragma region Awake/Pre/Update/Post/Clean

bool j1Colliders::Awake(pugi::xml_node & conf)
{
	scale = App->win->GetScale();
	size = App->render->drawsize;
	
	return true;
}

bool j1Colliders::PreUpdate()
{
	p2List_item<Collider*>* collider = colliders.start;
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < colliders.count(); ++i)
	{
		if (collider != nullptr && collider->data->to_delete == true)
		{
			colliders.del(collider);
		}

		collider = collider->next;
	}

	p2List_item<Collider*>* detected_collider = detected_Colliders.start;
	// Disable all colliders in the list
	for (uint i = 0; i < detected_Colliders.count(); ++i)
	{
		if (App->player->GetPlayerCollider()->rect.y + 30 < detected_collider->data->rect.y) {
			detected_collider->data->Enabled = true;
			detected_Colliders.del(detected_collider);
		}
		detected_collider = detected_collider->next;

	}
	return true;
}

bool j1Colliders::Update(float dt)
{
	Draw();
	return true;
}

bool j1Colliders::PostUpdate()
{
	return true;
}

bool j1Colliders::CleanUp()
{
	//Clean all colliders
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

#pragma region Render

void j1Colliders::Draw()
{
	if (collider_debug)
	{
		p2List_item<Collider*>* collider = this->colliders.start;

		for (int i = 0; i < colliders.count(); i++)
		{
			SDL_Rect rect = { 0,0,0,0 };
			rect.x =  collider->data->rect.x * size * scale;
			rect.y = collider->data->rect.y * size * scale;
			rect.w = collider->data->rect.w * size * scale;
			rect.h = collider->data->rect.h * size * scale;
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
				App->render->DrawQuad(rect, 0, 255, 0, 100);
				break;
			case COLLIDER_WINDOW:
				App->render->DrawQuad(rect, 0, 255, 255, 50);
				break;
			case COLLIDER_CAMERA:
				App->render->DrawQuad(rect, 0, 0, 0, 50, false);
				break;
			case COLLIDER_EXIT:
				App->render->DrawQuad(rect, 100, 200, 100, 100);
				break;
			}
				
			

			if (collider->next != nullptr)
			{
				collider = collider->next;
			}
		}
	}
}

#pragma endregion

#pragma region AddFunctions

Collider* j1Colliders::AddCollider(SDL_Rect rect, ColliderType type,p2Point<int>offset, j1Module* callback, int Damage)
{	
	c = new Collider(rect, type,offset, callback, Damage);
	colliders.add(c);

	return c;
}
#pragma endregion

#pragma region CheckFunctions

bool j1Colliders::CheckColliderCollision(Collider* c1,ColliderType ignoredCollider,int* posY)
{
	
	bool ret = false;
	Collider* c2 = nullptr;
	p2List_item<Collider*>* c = colliders.start;
	c2 = c->data;
	for (uint i = 0; i < colliders.count(); i++)
	{
		if (c1 != c2 && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_CAMERA && c2->type != COLLIDER_EXIT) {


			if (c2->type == COLLIDER_DEAD)
			{
				if (c1->CheckCollision(c2->rect))
				{
						App->audio->PlayFx(1);
						App->fade2black->FadeToBlack(App->scene, App->scene);
						return false;
				}
						
			}
			if (c2->type == COLLIDER_WALL_TRASPASSABLE) {
				if (c1->CheckCollision(c2->rect) ) {
					if (c1->rect.y + c1->rect.h/2 < c2->rect.y && c2->Enabled == true) {
 						ret = true;
					}
					else {
						ret = false;
						c2->Enabled = false;
						if(detected_Colliders.find(c2) == -1)
						detected_Colliders.add(c2);
					}
					App->player->onPlatform = true;
				}
			}			

			else {
				ret = c1->CheckCollision(c2->rect);
				App->player->onPlatform = false;

			}
			if (ret) {

				if (c1->callback) {
					c1->callback->OnCollision(c1, c2);
					if (posY != nullptr)
						*posY = c2->rect.y;
				}
				return ret;
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
		if (c1 != c2 && c2->type != COLLIDER_WINDOW && c2->type != COLLIDER_CAMERA) {

			if (c2->type == COLLIDER_WALL_TRASPASSABLE) {
				if (c1->CheckCollision(c2->rect)) {
						ret = false;
						c2->Enabled = false;
						if (detected_Colliders.find(c2) == -1)
							detected_Colliders.add(c2);				
				}
			}
			else {
				ret = c1->CheckCollision(c2->rect);
			}
				return ret;
			
		}
		if (c->next != NULL)
			c = c->next;
		c2 = c->data;
	}

	return ret;
}


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