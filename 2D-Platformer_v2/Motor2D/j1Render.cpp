#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Tilesets.h"
#include "j1Colliders.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	pugi::xml_node camera_attr = config;
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	return true;
}

bool j1Render::PostUpdate()
{
//	LOG("CameraY: %i",camera.y);
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}


void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetCameraPos(pugi::xml_node& object)
{
	if (App->scene->loading == false)
	{

		if (App->scene->swapping)
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) //EUDALD: Check this when changing start cameras and cullings
			{
				SetCameraPosFromCurrentLevel(it);
			}
		}
		else
		{
			for (pugi::xml_node it = object.child("properties").child("property"); it; it = it.next_sibling("property")) //EUDALD: Check this when changing start cameras and cullings
			{	
					if (strcmp(it.attribute("name").as_string(), "camera_pos_x") == 0)
					{
						camera.x = it.attribute("value").as_int();
					}
					if (strcmp(it.attribute("name").as_string(), "camera_pos_y") == 0)
					{
						camera.y = it.attribute("value").as_int();
					}
			}
		}
		
	}
}

void j1Render::SetCameraPos(p2Point<int> pos)
{
	App->tiles->culling_Collider->rect.x = pos.x - (int)(App->tiles->culling_Collider->rect.w * 0.5 - App->entity->RetreivePlayerData().tile_width *.5f);
	App->tiles->culling_Collider->rect.y = pos.y - (int)(App->tiles->culling_Collider->rect.h * 0.5);

	camera.x = App->tiles->culling_Collider->rect.x * -2;
	camera.y = App->tiles->culling_Collider->rect.y * -2;

}

void j1Render::SetCameraPosFromCurrentLevel(pugi::xml_node& it)
{
	char* pos_x = "camera_pos_x";
	char* pos_y = "camera_pos_y";
	if (App->scene->current_level == App->scene->A1)
	{
		pos_x = "camera_pos_x_swap_from_A1";
		pos_y = "camera_pos_y_swap_from_A1";
	}
	else if (App->scene->current_level == App->scene->A2)
	{
		pos_x = "camera_pos_x_swap_from_A2";
		pos_y = "camera_pos_y_swap_from_A2";
	}
	else if (App->scene->current_level == App->scene->A3)
	{
		pos_x = "camera_pos_x_swap_from_A3";
		pos_y = "camera_pos_y_swap_from_A3";
	}
	else if (App->scene->current_level == App->scene->A5)
	{
		pos_x = "camera_pos_x_swap_from_A5";
		pos_y = "camera_pos_y_swap_from_A5";
	}

	if (strcmp(it.attribute("name").as_string(), pos_x) == 0)
	{
		camera.x = it.attribute("value").as_int();
	}
	if (strcmp(it.attribute("name").as_string(), pos_y) == 0)
	{
		camera.y = it.attribute("value").as_int();
	}
}

void j1Render::MoveCamera(p2Point<int> speed)
{
	App->render->camera.x += speed.x;
	App->tiles->culling_Collider->rect.x -= (int)roundf( speed.x * 0.5f);

	App->render->camera.y += speed.y;
	App->tiles->culling_Collider->rect.y -= (int)roundf( speed.y * 0.5f);
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section,float size,bool parallax,SDL_RendererFlip flip, float speed, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = App->win->GetScale();
	//int posX = x - App->player->pivot_x_flip;
	//if (flip == SDL_FLIP_HORIZONTAL) {
	//	x = posX;
	//}

	SDL_Rect rect;
		rect.x = (int)(camera.x * speed) + x * scale * size;
		rect.y = (int)(camera.y) + y * scale * size;
	if(section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}
	if (parallax) {
		size = 3.f;
	}
	rect.w *= scale * size;
	rect.h *= scale * size;
	
	SDL_Point* p = NULL;
	SDL_Point pivot;

	if(pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}


	if(SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x  + rect.x * scale);
		rec.y = (int)(camera.y  + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale );
	ret.y = (y - camera.y / scale  );

	return ret;
}