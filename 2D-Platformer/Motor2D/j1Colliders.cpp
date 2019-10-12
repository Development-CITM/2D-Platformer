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
	int scale = App->win->GetScale();
	int size = App->render->drawsize;
	ShowColliders();
	if (collider_debug)
	{
		p2List_item<Collider*>* collider = this->colliders.start;
		SDL_Rect collider_rect;
		for (int i = 0; i < colliders.count(); i++)
		{
			collider_rect.x = collider->data->x * scale * size;
			collider_rect.y = collider->data->y * scale * size;
			collider_rect.w = collider->data->width * scale * size;
			collider_rect.h = collider->data->height * scale * size;

			switch (collider->data->type)
			{
			case BARRIER:
				App->render->DrawQuad(collider_rect, 255, 0, 0, 40, true, true);
				break;
			case JUMPABLE:
				App->render->DrawQuad(collider_rect, 0, 0, 255, 100, true, true);
				break;
			case DEAD:
				App->render->DrawQuad(collider_rect, 0, 0, 0, 100, true, true);
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
	bool ret = true;
	for (object; object && ret; object = object.next_sibling("object"))
	{
		Collider* collider = new Collider();

		ret = LoadObject(object, collider);

		if (ret == true)
		{
			collider_loaded = ret;
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
	collider->x = node.attribute("x").as_int();
	collider->y = node.attribute("y").as_int();
	collider->width = node.attribute("width").as_int();
	collider->height = node.attribute("height").as_int();

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

