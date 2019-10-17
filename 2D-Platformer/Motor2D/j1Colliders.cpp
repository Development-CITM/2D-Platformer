#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1Player.h"
#include <math.h>

j1Colliders::j1Colliders():j1Module()
{
	name.create("colliders");

	//collider_matrix[COLLIDER_PLAYER][COLLIDER_WALL_SOLID] = true;
	//collider_matrix[COLLIDER_PLAYER][COLLIDER_WALL_TRASPASSABLE] = true;
	//collider_matrix[COLLIDER_PLAYER][COLLIDER_DEAD] = true;

}

j1Colliders::~j1Colliders()
{}

bool j1Colliders::Awake(pugi::xml_node & conf)
{
	scale = App->win->GetScale();
	size = App->render->drawsize;
	return true;
}

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
				App->render->DrawQuad(rect, 255, 0, 0, 40, true, true);
				break;
			case COLLIDER_WALL_TRASPASSABLE:
				App->render->DrawQuad(rect, 0, 0, 255, 100, true, true);
				break;
			case COLLIDER_DEAD:
				App->render->DrawQuad(rect, 0, 0, 0, 100, true, true);
				break;
			case COLLIDER_PLAYER:
				App->render->DrawQuad(rect, 0, 255, 0, 100, true, true);
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
	// Remove all colliders scheduled for deletion
	//for (uint i = 0; i < colliders.count(); ++i)
	//{
	//	if (colliders[i] != nullptr && colliders[i]->to_delete == true)
	//	{
	//		delete colliders[i];
	//		colliders[i] = nullptr;
	//	}
	//}

	// Calculate collisions
	Collider* c1;
	Collider* c2;
	p2List_item<Collider*>* collider = colliders.start;

	for (uint i = 0; i < colliders.count(); ++i)
	{
		if (colliders[i]->Enabled == true)
		{
			c1 = collider->data;
			
			// avoid checking collisions already checked
			for (uint k = i + 1; k < colliders.count(); ++k)
			{
				// skip empty colliders
				if (colliders[k] == nullptr)
					continue;
				c2 = colliders[k];

				if (c1->CheckCollision(c2->rect) == true)
				{
					if (CheckCollisionTypes(c1, c2) && c1->callback) {
						c1->callback->OnCollision();
					}
					//if (matrix[c1->type][c2->type] && c1->callback)
					//	c1->callback->OnCollision(c1, c2);

					//if (matrix[c2->type][c1->type] && c2->callback)
					//	c2->callback->OnCollision(c2, c1);
				}
			}
		}


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


	return ret;
}

//Collider* j1Colliders::CreateCollider(SDL_Rect* collider,p2Point<int> pos, int type)
//{
//	ColliderType collType;
//	Collider* c = new Collider();
//
//	c->offset.y = collider->y;
//	c->offset.x = collider->x;
//
//	c->rect = { pos.x + c->offset.x ,pos.y + c->offset.y,collider->w,collider->h };
//	switch (type)
//	{
//	case 1:
//		collType = COLLIDER_PLAYER;
//		c->type = collType;
//		colliders.add(c);
//	default:
//		break;
//	}
//
//	return c;
//}

Collider* j1Colliders::AddCollider(SDL_Rect rect, ColliderType type, j1Module* callback, int Damage)
{
	Collider* c = nullptr;

	c = new Collider(rect, type, callback, Damage);
	colliders.add(c);

	return c;

}


bool j1Colliders::CheckCollisionTypes(Collider* c1, Collider* c2)
{
	bool ret = false;
	switch (c1->type)
	{
	case COLLIDER_PLAYER:
		ret = true;
		switch (c2->type)
		{
		case COLLIDER_WALL_SOLID:
			ret = true;
			LOG("PLAYER-WALL_SOLID");
			break;
		}
		break;
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



