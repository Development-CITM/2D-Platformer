#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Window.h"
#include "j1Input.h"
#include <math.h>

j1Colliders::j1Colliders():j1Module()
{
	name.create("colliders");
}

j1Colliders::~j1Colliders()
{}

bool j1Colliders::Awake(pugi::xml_node & conf)
{
	return true;
}

void j1Colliders::Draw()
{
	ShowColliders();
	if (collider_debug)
	{
		p2List_item<Collider*>* collider = this->colliders.start;

		for (int i = 0; i < colliders.count(); i++)
		{
			switch (collider->data->type)
			{
			case BARRIER:
				App->render->DrawQuad(collider->data->rect, 255, 0, 0, 40, true, true);
				break;
			case JUMPABLE:
				App->render->DrawQuad(collider->data->rect, 0, 0, 255, 100, true, true);
				break;
			case DEAD:
				App->render->DrawQuad(collider->data->rect, 0, 0, 0, 100, true, true);
				break;
			}

			if (collider->next != nullptr)
			{
				collider = collider->next;
			}
		}
	}
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

bool j1Colliders::Load(pugi::xml_node object)
{
	scale = App->win->GetScale();
	size = App->render->drawsize;
	bool ret = true;
	
		for (object.child("object"); object && ret; object = object.next_sibling("object"))
		{
			Collider* collider = new Collider();

			ret = LoadObject(object, collider);

			if (ret == true)
			{

				colliders.add(collider);
			}
		}
	
	return ret;
}

bool j1Colliders::PreUpdate()
{
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


bool j1Colliders::LoadObject(pugi::xml_node& node, Collider* collider)
{
	bool ret = true;
	collider->rect.x = node.attribute("x").as_int() * size * scale;
	collider->rect.y = node.attribute("y").as_int() * size * scale;
	collider->rect.w = node.attribute("width").as_int() * size * scale;
	collider->rect.h = node.attribute("height").as_int() * size * scale;

	if (strcmp(node.attribute("type").as_string(), "Jumpable") == 0)
	{
		collider->type = JUMPABLE;
	}
	if (strcmp(node.attribute("type").as_string(), "Dead") == 0)
	{
		collider->type = DEAD;
	}
	if (strcmp(node.attribute("type").as_string(), "Barrier") == 0)
	{
		collider->type = BARRIER;
	}

	return ret;
}

void j1Colliders::ShowColliders()
{

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && collider_debug == false)
	{
		collider_debug = true;
	}
	else if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN && collider_debug == true)
	{
		collider_debug = false;
	}
}

