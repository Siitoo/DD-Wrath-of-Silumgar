#include "ctApp.h"
#include "ctRender.h"
#include "ctEntities.h"
#include "ctTextures.h"
#include "Entity.h"
#include "ctAudio.h"
#include "ctWindow.h"
#include "ctLog.h"
#include "ctFadeToBlack.h"

#include "Cleric.h"
#include "Dwarf.h"
#include "Elf.h"
#include "Warrior.h"


ctEntities::ctEntities()
{
	name = "entities";
}

// Destructor
ctEntities::~ctEntities()
{
	LOG("Unloading entities spritesheet");
	App->tex->UnLoad(entity_sprites);
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
	
	return ret;
}

bool ctEntities::Start()
{
	bool ret = true;

	//cleric spritesheet
	cleric_spritesheet = App->tex->Load(cleric_spritesheet_name.data());
	dwarf_spritesheet = App->tex->Load(dwarf_spritesheet_name.data());
	elf_spritesheet = App->tex->Load(elf_spritesheet_name.data());
	warrior_spritesheet = App->tex->Load(warrior_spritesheet_name.data());

	if (cleric_spritesheet == NULL|| dwarf_spritesheet == NULL || elf_spritesheet == NULL || warrior_spritesheet == NULL) {
		LOG("Error loading entities spritesheet!!");
		ret = false;
	}
	
	if (!ret)
		return false;

	return ret;
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
	return true;
}

// Called before render is available
bool ctEntities::Update(float dt)
{

	for (int i = 0; i < entities.size(); i++)
		if (entities.at(i) != nullptr) entities[i]->Update(dt);

	for (int i = 0; i < entities.size(); i++) {
		switch (entities.at(i)->type)
		{
		case CLERIC:
			if (entities.at(i) != nullptr) entities[i]->Draw(cleric_spritesheet);
			break;
		case DWARF:
			if (entities.at(i) != nullptr) entities[i]->Draw(dwarf_spritesheet);
			break;
		case ELF:
			if (entities.at(i) != nullptr) entities[i]->Draw(elf_spritesheet);
			break;
		case WARRIOR:
			if (entities.at(i) != nullptr) entities[i]->Draw(warrior_spritesheet);
			break;
		default:
			break;
		}
		
	}
		

	return true;
}

// Called before quitting
bool ctEntities::CleanUp()
{
	LOG("Freeing all entities");

	//cleric spritesheet
	App->tex->UnLoad(cleric_spritesheet);
	App->tex->UnLoad(dwarf_spritesheet);
	App->tex->UnLoad(elf_spritesheet);
	App->tex->UnLoad(warrior_spritesheet);

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

bool ctEntities::SpawnEntity(int x, int y, EntityType type)
{
	// find room for the new entity
	bool ret = false;

	switch (type)
	{
	case EntityType::CLERIC: {
		Cleric* cleric = new Cleric(x, y, CLERIC);
		entities.push_back(cleric);
		ret = true;
		break;
	}
	case EntityType::DWARF: {
		Dwarf* dwarf = new Dwarf(x, y, DWARF);
		entities.push_back(dwarf);
		ret = true;
		break;
	}
	case EntityType::ELF: {
		Elf* elf = new Elf(x, y, ELF);
		entities.push_back(elf);
		ret = true;
		break;
	}
	case EntityType::WARRIOR: {
		Warrior* warrior = new Warrior(x, y, WARRIOR);
		entities.push_back(warrior);
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