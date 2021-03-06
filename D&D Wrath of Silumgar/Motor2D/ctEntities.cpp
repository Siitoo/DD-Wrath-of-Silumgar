#include "ctApp.h"
#include "ctRender.h"
#include "ctEntities.h"
#include "ctTextures.h"
#include "Entity.h"
#include "ctAudio.h"
#include "ctWindow.h"
#include "ctLog.h"
#include "ctCombat.h"
#include "ctFadeToBlack.h"
#include "j1Map.h"
#include "Cleric.h"
#include "Dwarf.h"
#include "Elf.h"
#include "Warrior.h"

#include "Kobold.h"
#include "Owlbear.h"
#include "Gnoll.h"
#include "GnollArcher.h"
#include "Goblin.h"
#include "GoblinHeavy.h"
#include "GoblinAlchemist.h"
#include "Triton.h"
#include "Hellhound.h"
#include "Skeleton.h"
#include "DarkWarrior.h"
#include "trollLeaders.h"
#include "DisplacerBeast.h"
#include "Lich2.h"
#include "Lich1.h"
#include "MiniHeroes.h"


ctEntities::ctEntities()
{
	name = "entities";
}

// Destructor
ctEntities::~ctEntities()
{
	LOG("Unloading entities spritesheet");
	
}

bool ctEntities::Awake(pugi::xml_node& config)
{
	LOG("Loading Entities from config file");
	bool ret = true;

	//cleric spritesheet
	cleric_spritesheet_name = config.child("cleric").attribute("spritesheetName").as_string();
	dwarf_spritesheet_name = config.child("dwarf").attribute("spritesheetName").as_string();
	elf_spritesheet_name = config.child("elf").attribute("spritesheetName").as_string();
	warrior_spritesheet_name = config.child("warrior").attribute("spritesheetName").as_string();

	kobold_spritesheet_name = config.child("kobold").attribute("spritesheetName").as_string();
	gnoll_spritesheet_name = config.child("gnollA").attribute("spritesheetName").as_string();
	gnollArcher_spritesheet_name = config.child("bowGnoll").attribute("spritesheetName").as_string();
	owlbear_spritesheet_name = config.child("owlbear").attribute("spritesheetName").as_string();
	goblins_spritesheet_name = config.child("goblins").attribute("spritesheetName").as_string();
	goblin_heavy_spritesheet_name = config.child("goblins").attribute("spritesheetName").as_string();
	triton_spritesheet_name = config.child("triton").attribute("spritesheetName").as_string();
	skeleton_spritesheet_name = config.child("skeleton").attribute("spritesheetName").as_string();
	hellhound_spritesheet_name = config.child("hellhound").attribute("spritesheetName").as_string();
	dark_warrior_spritesheet_name = config.child("darkWarrior").attribute("spritesheetName").as_string();
	trollLeader_spritesheet_name = config.child("trollLeader").attribute("spritesheetName").as_string();
	displacerBeast_spritesheet_name = config.child("displacerBeast").attribute("spritesheetName").as_string();
	lich2_spritesheet_name = config.child("lich2").attribute("spritesheetName").as_string();
	lich1_spritesheet_name = config.child("lich1").attribute("spritesheetName").as_string();
	miniheroes_spritesheet_name = config.child("miniheroes").attribute("spritesheetName").as_string();
	return ret;
}

bool ctEntities::Start()
{
	

	return true;
}

bool ctEntities::PreUpdate()
{
	for (int i = 0; i < entities.size(); i++) {
		if (entities[i]->to_destroy) {
			delete(entities[i]);
			entities[i] = nullptr;
			entities.erase(entities.cbegin() + i);
			entities.shrink_to_fit();
		}
	}

	if (entities.size() != draw_priority_entities.size())
	{
		draw_priority_entities = entities;
	}

	OrderDrawEntities();

	
	return true;
}

// Called before render is available
bool ctEntities::Update(float dt)
{
	for (int i = 0; i < entities.size(); i++)
		if (entities.at(i) != nullptr) entities[i]->Update(dt);

	for (int i = 0; i <draw_priority_entities.size(); i++)
		if (draw_priority_entities.at(i) != nullptr) draw_priority_entities[i]->Draw();


	return true;

}

// Called before quitting
bool ctEntities::CleanUp()
{
	LOG("Freeing all entities");



	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities[i] != nullptr)
		{
			delete entities[i];
			entities[i] = nullptr;
			entities.erase(entities.cbegin() + i);
			entities.shrink_to_fit();
		}
	}

	entities.clear();


	return true;
}


bool ctEntities:: SpawnEntity(int x, int y, EntityType type)
{
	// find room for the new entity
	bool ret = false;



	switch (type)
	{
	case EntityType::CLERIC: {
		Cleric* cleric = new Cleric(x, y, CLERIC);
		entities.push_back(cleric);
		App->combat->turn_priority_entity.push_back(cleric);
		ret = true;
		break;
	}
	case EntityType::DWARF: {
		Dwarf* dwarf = new Dwarf(x, y, DWARF);
		entities.push_back(dwarf);
		App->combat->turn_priority_entity.push_back(dwarf);
		ret = true;
		break;
	}
	case EntityType::ELF: {
		Elf* elf = new Elf(x, y, ELF);
		entities.push_back(elf);
		App->combat->turn_priority_entity.push_back(elf);
		ret = true;
		break;
	}
	case EntityType::WARRIOR: {
		Warrior* warrior = new Warrior(x, y, WARRIOR);
		entities.push_back(warrior);
		App->combat->turn_priority_entity.push_back(warrior);
		ret = true;
		break;
	}
	case EntityType::KOBOLD: {
		Kobold* kobold = new Kobold(x, y, KOBOLD);
		entities.push_back(kobold);
		App->combat->turn_priority_entity.push_back(kobold);
		ret = true;
		break;
	}
	case EntityType::GNOLL: {
		Gnoll* gnoll = new Gnoll(x, y, GNOLL);
		entities.push_back(gnoll);
		App->combat->turn_priority_entity.push_back(gnoll);
		ret = true;
		break;
	}
	case EntityType::GNOLL_ARCHER: {
		GnollArcher* gnollArcher = new GnollArcher(x, y, GNOLL_ARCHER);
		entities.push_back(gnollArcher);
		App->combat->turn_priority_entity.push_back(gnollArcher);
		ret = true;
		break;
	}
	case EntityType::OWLBEAR: {
		Owlbear* owlbear = new Owlbear(x, y, OWLBEAR);
		entities.push_back(owlbear);
		App->combat->turn_priority_entity.push_back(owlbear);
		ret = true;
		break;
	}
	case EntityType::GOBLIN: {
		Goblin* goblin = new Goblin(x, y, GOBLIN);
		entities.push_back(goblin);
		App->combat->turn_priority_entity.push_back(goblin);
		ret = true;
		break;
	}
	case EntityType::HEAVY_GOBLIN: {
		Goblin_Heavy* goblinHeavy = new Goblin_Heavy(x, y, HEAVY_GOBLIN);
		entities.push_back(goblinHeavy);
		App->combat->turn_priority_entity.push_back(goblinHeavy);
		ret = true;
		break;
	}
	case EntityType::ALCHEMIST_GOBLIN: {
		Goblin_Alchemist* goblinAlchemist = new Goblin_Alchemist(x, y, ALCHEMIST_GOBLIN);
		entities.push_back(goblinAlchemist);
		App->combat->turn_priority_entity.push_back(goblinAlchemist);
		ret = true;
		break;
	}
	case EntityType::HELLHOUND: {
		Hellhound* hellhound = new Hellhound(x, y, HELLHOUND);
		entities.push_back(hellhound);
		App->combat->turn_priority_entity.push_back(hellhound);
		ret = true;
		break;
	}
	case EntityType::SKELETON: {
		Skeleton* skeleton = new Skeleton(x, y, SKELETON);
		entities.push_back(skeleton);
		App->combat->turn_priority_entity.push_back(skeleton);
		ret = true;
		break;
	}
	case EntityType::TRITON: {
		Triton* triton = new Triton(x, y, TRITON);
		entities.push_back(triton);
		App->combat->turn_priority_entity.push_back(triton);
		ret = true;
		break;
	}
	case EntityType::DARK_WARRIOR: {
		DarkWarrior* darky = new DarkWarrior(x, y, DARK_WARRIOR);
		entities.push_back(darky);
		App->combat->turn_priority_entity.push_back(darky);
		ret = true;
		break;
	}

	case EntityType::TROLL_LEADERS: {
		TrollLeaders* TrollLead = new TrollLeaders(x, y, TROLL_LEADERS);
		entities.push_back(TrollLead);
		App->combat->turn_priority_entity.push_back(TrollLead);
		ret = true;
		break;
	}
	case EntityType::DISPLACER_BEAST: {
		DisplacerBeast* beast = new DisplacerBeast(x, y, DISPLACER_BEAST);
		entities.push_back(beast);
		App->combat->turn_priority_entity.push_back(beast);
		ret = true;
		break;
	}

	case EntityType::LICH_2: {
		Lich2* lich2 = new Lich2(x, y, LICH_2);
		entities.push_back(lich2);
		App->combat->turn_priority_entity.push_back(lich2);
		ret = true;
		break;
	}

	case EntityType::LICH_1: {
		Lich1* lich1 = new Lich1(x, y, LICH_1);
		entities.push_back(lich1);
		App->combat->turn_priority_entity.push_back(lich1);
		ret = true;
		break;
	}

	case EntityType::MINIHEROES: {
		MiniHeroes* miniheroes = new MiniHeroes(x, y, MINIHEROES);
		entities.push_back(miniheroes);
		ret = true;
		break;
	}

	default:
		break;
	}


	return ret;
}


Cleric* ctEntities::GetCleric() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == CLERIC)
				return (Cleric*)entities[i];
		}
	}

	return nullptr;

}
Dwarf* ctEntities::GetDwarf() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == DWARF)
				return (Dwarf*)entities[i];
		}
	}

	return nullptr;

}
Elf* ctEntities::GetElf() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == ELF)
				return (Elf*)entities[i];
		}
	}

	return nullptr;

}
Warrior* ctEntities::GetWarrior() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == WARRIOR)
				return (Warrior*)entities[i];
		}
	}

	return nullptr;

}
Kobold* ctEntities::GetKobold() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == KOBOLD)
				return (Kobold*)entities[i];
		}
	}

	return nullptr;

}
Gnoll* ctEntities::GetGnoll() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == GNOLL)
				return (Gnoll*)entities[i];
		}
	}

	return nullptr;

}
gnollArcher* ctEntities::GetGnoll_Archer() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == GNOLL_ARCHER)
				return (gnollArcher*)entities[i];
		}
	}

	return nullptr;

}
Owlbear* ctEntities::GetOwlbear() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == OWLBEAR)
				return (Owlbear*)entities[i];
		}
	}

	return nullptr;

}
Goblin* ctEntities::GetGoblin() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == GOBLIN)
				return (Goblin*)entities[i];
		}
	}

	return nullptr;

}
Goblin_Heavy* ctEntities::GetHeavyGoblin() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == HEAVY_GOBLIN)
				return (Goblin_Heavy*)entities[i];
		}
	}

	return nullptr;

}
Goblin_Alchemist* ctEntities::GetAlchemistGoblin() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == ALCHEMIST_GOBLIN)
				return (Goblin_Alchemist*)entities[i];
		}
	}

	return nullptr;

}

Triton* ctEntities::GetTriton() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == TRITON)
				return (Triton*)entities[i];
		}
	}

	return nullptr;

}

Skeleton* ctEntities::GetSkeleton() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == SKELETON)
				return (Skeleton*)entities[i];
		}
	}

	return nullptr;

}

Hellhound* ctEntities::GetHellhound() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == HELLHOUND)
				return (Hellhound*)entities[i];
		}
	}

	return nullptr;

}

Lich2* ctEntities::GetLich() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == LICH_2)
				return (Lich2*)entities[i];
		}
	}

	return nullptr;

}


MiniHeroes* ctEntities::GetMiniheroes() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == MINIHEROES)
				return (MiniHeroes*)entities[i];
		}
	}

	return nullptr;

}

DarkWarrior* ctEntities::GetDarkWarrior() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == DARK_WARRIOR)
				return (DarkWarrior*)entities[i];
		}
	}

	return nullptr;

}

TrollLeaders* ctEntities::GetTrollLeader() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == TROLL_LEADERS)
				return (TrollLeaders*)entities[i];
		}
	}

	return nullptr;

}

DisplacerBeast* ctEntities::GetDisplacerBeast() const {

	for (uint i = 0; i < entities.size(); ++i)
	{
		if (entities.at(i) != nullptr)
		{
			if (entities[i]->type == DISPLACER_BEAST)
				return (DisplacerBeast*)entities[i];
		}
	}

	return nullptr;

}

void ctEntities::OrderDrawEntities()
{
	bool ordered = false;
	
	std::vector<Entity*> order_entity = draw_priority_entities;

	while (!ordered)
	{
   		ordered = true;
		std::vector<Entity*>::iterator itnext = order_entity.begin();
		int count = 0;
		for (std::vector<Entity*>::iterator it = order_entity.begin(); it != order_entity.end(); ++it)
		{
			itnext++;
			count++;
			if (count != order_entity.size())
			{
				if ((*it)->position.y > (*itnext)->position.y )
				{
					Entity* entity_tmp = (*it);

					(*it) = (*itnext);
					it++;
					(*it) = entity_tmp;
					it--;
					ordered = false;

				}
				else if ((*it)->position.y == (*itnext)->position.y)
				{

					if (((*itnext)->type != CLERIC && (*itnext)->type != WARRIOR && (*itnext)->type != ELF && (*itnext)->type != DWARF) && ((*it)->type == CLERIC || (*it)->type == WARRIOR || (*it)->type == ELF || (*it)->type == DWARF))
					{
						Entity* entity_tmp = (*it);

						(*it) = (*itnext);
						it++;
						(*it) = entity_tmp;
						it--;
						ordered = false;
					}
				}
				
			}
			else {
				break;
			}

		}

	}
	ordered = false;





	draw_priority_entities = order_entity;

	order_entity.clear();
}

Entity* ctEntities::GetActor(int actor_code) {

	switch (actor_code) {
	case CLERIC:
		return GetCleric();
		break;
	case WARRIOR:
		return GetWarrior();
		break;
	case LICH_2:
		return GetLich();
		break;
	default:
		return nullptr;
		break;
	}

}

