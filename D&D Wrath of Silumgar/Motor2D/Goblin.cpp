#include "ctDefs.h"
#include "ctLog.h"
#include "ctApp.h"
#include "ctInput.h"
#include "ctRender.h"
#include "ctTextures.h"
#include "ctFadeToBlack.h"
#include "ctAudio.h"
#include "ctEntities.h"
#include "ctCombat.h"
#include "ctTaskManager.h"

#include "Goblin.h"


Goblin::Goblin(int x, int y, EntityType type) : Entity(x, y, type) {

	bool ret = true;

	pugi::xml_document	config_file;
	pugi::xml_node* node = &App->LoadEntities(config_file);
	node = &node->child("enemies").child("goblin");
	texture = App->tex->Load(App->entities->goblin_spritesheet_name.data());

	for (pugi::xml_node animations = node->child("animations").child("animation"); animations && ret; animations = animations.next_sibling("animation"))
	{
		std::string tmp(animations.attribute("name").as_string());

		if (tmp == "idle")
			LoadAnimation(animations, &idle);
		else if (tmp == "run")
			LoadAnimation(animations, &run_forward);
		else if (tmp == "axe_attack")
			LoadAnimation(animations, &attack);
		else if (tmp == "hit")
			LoadAnimation(animations, &hit);
		else if (tmp == "death")
			LoadAnimation(animations, &death);
		else if (tmp == "stun")
			LoadAnimation(animations, &stun);
		else if (tmp == "dodge")
			LoadAnimation(animations, &dodge);
	}
	LoadProperties(node->child("statistics"));
	animation = &hit;

}



// Called each loop iteration
void Goblin::Update(float dt)
{

	if (dt > 0)
	{
		if (!key_entities_speed)
			SetEntitiesSpeed(dt);

		SetPlayerAnimationsSpeed(dt);
	}

}

void Goblin::SetPlayerAnimationsSpeed(float dt)
{
	idle.speed = idle_vel * dt;
	run_forward.speed = run_forward_vel * dt;
}

void Goblin::SetEntitiesSpeed(float dt)
{
	idle_vel = idle.speed;
	run_forward_vel = run_forward.speed;

	key_entities_speed = true;
}

void Goblin::LoadAnimation(pugi::xml_node animation_node, ctAnimation* animation)
{
	bool ret = true;

	for (pugi::xml_node frame = animation_node.child("frame"); frame && ret; frame = frame.next_sibling("frame"))
		animation->PushBack({ frame.attribute("x").as_int() , frame.attribute("y").as_int(), frame.attribute("width").as_int(), frame.attribute("height").as_int() });

	animation->speed = animation_node.attribute("speed").as_float();
	animation->loop = animation_node.attribute("loop").as_bool();
}

void Goblin::Attack()
{
	App->audio->PlayFx(App->audio->kobold_attack_fx, 0);

}

void  Goblin::Death() {
	App->audio->PlayFx(App->audio->kobold_death_fx, 0);
}
void  Goblin::Run() {
	App->audio->PlayFx(App->audio->kobold_run_fx, 0);
}
void  Goblin::Damaged() {
	App->audio->PlayFx(App->audio->kobold_damaged_fx, 0);
}

void Goblin::PerformAction()
{
	Entity* entity_objective = nullptr;
	if (IsGoingToDoAnythingClever()) {
		//in this case the kobold will search the weakest heroe since we dont have abilities
		entity_objective = App->combat->GetTheWeakestHeroe();
	}
	else {
		//in this case, the kobold will attack one random heroe
		entity_objective = App->combat->GetRandomHeroe();
	}

	App->task_manager->AddTask(new MoveToEntity(this, entity_objective, 20));
	App->task_manager->AddTask(new PerformActionToEntity(this, this->default_attack, entity_objective));
	App->task_manager->AddTask(new MoveToInitialPosition(this));
}