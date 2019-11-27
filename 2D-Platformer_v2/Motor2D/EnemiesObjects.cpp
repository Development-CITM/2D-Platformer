#include "EnemiesObjects.h"
#include "p2Log.h"
#include "j1Colliders.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Pathfinding.h"
#include "j1Tilesets.h"
#include "j1Scene.h"

Object_Enemy::Object_Enemy(Object_type type, p2Point<int> pos) : Object_Character()
{
	characterPos = pos;
	type_object = type;
	if (type == Object_type::ENEMY_GROUND)
	{
		Load("animations/Enemy_Kobold.tmx");
		collider = App->collider->AddCollider({ characterPos.x + 56,characterPos.y + 5,25,46 }, COLLIDER_ENEMY);
	}
	else if (type == Object_type::ENEMY_FLYING)
	{
		Load("animations/Enemy_Wwisp.tmx");
		collider = App->collider->AddCollider({ characterPos.x+25 ,characterPos.y + 12,25,25 }, COLLIDER_ENEMY);
	}
	currentAnimation = idle;
}

Object_Enemy::~Object_Enemy()
{
}

bool Object_Enemy::Start()
{
	bool ret = true;
	
	
	
	
	return ret;
}

bool Object_Enemy::PreUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Enemy::Update(float dt)
{
	bool ret = true;
	StablishPath();
	//move
	Draw(dt);
	return ret;
}

bool Object_Enemy::PostUpdate()
{
	bool ret = true;
	return ret;
}

bool Object_Enemy::CleanUp()
{
	bool ret = true;
	
	return ret;
}

void Object_Enemy::StablishPath()
{
	iPoint origin;
	iPoint player_pos;

	origin.x = characterPos.x;
	origin.y = characterPos.y;

	player_pos.x = App->player->GetCollider()->GetPosition().x;
	player_pos.y = App->player->GetCollider()->GetPosition().y;

	origin = App->tiles->WorldToMap(origin.x, origin.y);
	player_pos = App->tiles->WorldToMap(player_pos.x, player_pos.y);

	if (origin.DistanceNoSqrt(player_pos) < 100) {
		App->pathfinding->CreatePath(origin, player_pos);
	}
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		if (origin.DistanceNoSqrt(player_pos) < 100) {
			iPoint pos = App->tiles->MapToWorld(path->At(i)->x, path->At(i)->y);
			App->render->Blit(App->scene->debug_tex, pos.x, pos.y, NULL, App->render->drawsize);
		}
	}
	if (path->At(1)) {
		MoveToTarget(App->tiles->MapToWorld(path->At(1)->x, path->At(1)->y));
		
		LOG("Path(0) (%i,%i)", App->tiles->MapToWorld(path->At(0)->x, path->At(0)->y).x, App->tiles->MapToWorld(path->At(0)->x, path->At(0)->y).y);
		LOG("Path(1) (%i,%i)", App->tiles->MapToWorld(path->At(1)->x, path->At(1)->y).x, App->tiles->MapToWorld(path->At(1)->x, path->At(1)->y).y);
		LOG("Enemy (%i,%i)", App->tiles->MapToWorld(origin.x, origin.y).x, App->tiles->MapToWorld(origin.x, origin.y).y);
		LOG("Enemy pos: (%i,%i)", characterPos.x, characterPos.y);
	}
}

void Object_Enemy::MoveToTarget(p2Point<int> target)
{
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	if (path->At(1) != nullptr) {
		if (characterPos.x < target.x) {
			characterPos.x += 2;
		}
		else if (characterPos.x > target.x) {
			characterPos.x -= 2;
		}
		else if(characterPos.y < target.y) {
			characterPos.y += 2;
		}
		else if(characterPos.y > target.y){
			characterPos.y -= 2;
		}

	}
}

Animation* Object_Enemy::LoadAnimation(pugi::xml_node& obj_group)
{
	Animation* anim = new Animation();
	anim->name = obj_group.attribute("name").as_string();

	if (strcmp(anim->name.GetString(), "IDLE") == 0) { idle = anim; }
	else if (strcmp(anim->name.GetString(), "RUN") == 0) { running = anim; running->offset.x = 5; }
	else if (strcmp(anim->name.GetString(), "HURT") == 0) { hurt = anim; hurt->loop = false; }
	else if (strcmp(anim->name.GetString(), "DEAD") == 0) { dead = anim; dead->loop = false; }

	anim->num_sprites = obj_group.child("properties").child("property").last_attribute().as_int();

	anim->sprites = new Sprite[anim->num_sprites];
	int i = 0;
	pugi::xml_node AABB_object = obj_group.next_sibling("objectgroup").child("object");

	for (pugi::xml_node object = obj_group.child("object"); object; object = object.next_sibling("object"))
	{
		anim->sprites[i].rect.x = object.attribute("x").as_int();
		anim->sprites[i].rect.y = object.attribute("y").as_int();
		anim->sprites[i].rect.w = object.attribute("width").as_int();
		anim->sprites[i].rect.h = object.attribute("height").as_int();

		anim->sprites[i].frames = object.child("properties").child("property").attribute("value").as_int();
		i++;
	}
	return anim;
}
