#include "ctDefs.h"
#include "ctLog.h"
#include "ctApp.h"
#include "ctInput.h"
#include "ctTextures.h"
#include "ctAudio.h"
#include "ctRender.h"
#include "ctWindow.h"
#include "ctEntities.h"
#include "ctTaskManager.h"

#include "ctCombat.h"
#include "ctWorldMap.h"
#include "j1Map.h"
#include "ctMainMenu.h"

#include "Cleric.h"
#include "Dwarf.h"
#include "Elf.h"
#include "Warrior.h"

#include "ctGui.h"

#include "ctFadeToBlack.h"

//randomize libs
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */



ctCombat::ctCombat() : ctModule()
{
	name = "combat";
}

// Destructor
ctCombat::~ctCombat()
{}

// Called before render is available

bool ctCombat::Awake(pugi::xml_node& config)
{
	LOG("Loading Combat");
	bool ret = true;

	/* initialize random seed: */
	srand(time(NULL));

	return ret;
}

// Called before the first frame
bool ctCombat::Start()
{
	bool ret = true;

	//Displaying map
	App->map->sceneName = this->scene_name;
	LOG("%s", App->map->sceneName.c_str());
	App->map->Load(App->map->sceneName.c_str());
	App->map->LayersSetUp();
	App->map->setAllLogicForMap();

	cleric_background = App->gui->AddUIImage(-1, -1, {0,0,242,31}, this, nullptr);
	warrior_background = App->gui->AddUIImage(242, -1, { 242,31,242,31 }, this, nullptr);
	elf_background = App->gui->AddUIImage(-1, 293, { 0,31,242,31 }, this, nullptr);
	dwarf_background = App->gui->AddUIImage(242, 293, { 242,0,242,31 }, this, nullptr);
	cleric_name = App->gui->AddUILabel(200, 3, "Cleric", { 255,255,255,255 }, 15, this);
	warrior_name = App->gui->AddUILabel(438, 3, "Warrior", { 255,255,255,255 }, 15, this);
	elf_name = App->gui->AddUILabel(210, 296, "Elf", { 255,255,255,255 }, 15, this);
	dwarf_name = App->gui->AddUILabel(443, 296, "Dwarf", { 255,255,255,255 }, 15, this);
	
	SpawnEntities();

	if (!App->main_menu->is_new_game) {
		//todo load from data.xml the current health, mana, items that have the heroes
		LoadDataFromXML();
	}

	SetDataToUI();

	OrderTurnPriority();
	
	return ret;
}

// Called each loop iteration
bool ctCombat::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool ctCombat::Update(float dt)
{

	if (turn_priority_entity.size() == 0) {
		OrderTurnPriority();
	}
	else {
		Entity* entity_to_perform_action = turn_priority_entity.front();
		
		PerformActionWithEntity(entity_to_perform_action);
	}
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && App->fadeToBlack->FadeIsOver())
		App->fadeToBlack->FadeToBlackBetweenModules(this, App->world_map, 1.0f);


	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->task_manager->AddTask(new MoveForward(App->entities->GetCleric(), {App->entities->GetCleric()->position.x + 250,App->entities->GetCleric()->position.y}));
		App->task_manager->AddTask(new Attack(App->entities->GetCleric()));
		App->task_manager->AddTask(new MoveBackward(App->entities->GetCleric(), { App->entities->GetCleric()->position.x ,App->entities->GetCleric()->position.y }));
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->task_manager->AddTask(new MoveForward(App->entities->GetWarrior(), { App->entities->GetWarrior()->position.x + 250, App->entities->GetWarrior()->position.y }));
		App->task_manager->AddTask(new Attack(App->entities->GetWarrior()));
		App->task_manager->AddTask(new MoveBackward(App->entities->GetWarrior(), { App->entities->GetWarrior()->position.x, App->entities->GetWarrior()->position.y }));
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->task_manager->AddTask(new MoveForward(App->entities->GetElf(), { App->entities->GetElf()->position.x + 250,App->entities->GetElf()->position.y }));
		App->task_manager->AddTask(new Attack(App->entities->GetElf()));
		App->task_manager->AddTask(new MoveBackward(App->entities->GetElf(), { App->entities->GetElf()->position.x,App->entities->GetElf()->position.y }));
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->task_manager->AddTask(new MoveForward(App->entities->GetDwarf(), { App->entities->GetDwarf()->position.x + 250,App->entities->GetDwarf()->position.y }));
		App->task_manager->AddTask(new Attack(App->entities->GetDwarf()));
		App->task_manager->AddTask(new MoveBackward(App->entities->GetDwarf(), { App->entities->GetDwarf()->position.x,App->entities->GetDwarf()->position.y }));
	}

	// ZOOM

	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		App->render->scale_factor += 0.1;
	}
	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		App->render->scale_factor -= 0.1;
	}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		uint x = 0;

		x = CalculatedDamage(App->entities->GetCleric(), App->entities->GetWarrior());
		test->LowerBar(x);

		if (test->CurrentQuantity() <= 0)
		{
			App->fadeToBlack->FadeToBlackBetweenModules(this, App->world_map, 1.0f);
		}
		

		LOG("%i", x);

	}
	// Draw everything --------------------------------------
	App->map->Draw();
	DrawTurnPriority();
	return true;
}

// Called each loop iteration
bool ctCombat::PostUpdate()
{
	bool ret = true;


	return ret;
}

// Called before quitting
bool ctCombat::CleanUp()
{
	LOG("Freeing combat");

	//todo: despawn entities
	
	turn_priority_entity.clear();
	App->gui->DeleteAllUIElements();

	App->map->CleanUp();

	if (App->main_menu->is_new_game)
		App->main_menu->is_new_game = false;

	if(App->entities->entities.size()>0)
		SaveDataToXML();

	for (int i = 0; i < App->entities->entities.size(); i++)
	{
		App->entities->entities.at(i)->to_destroy = true;
	}
	App->entities->entities.clear();

	return true;
}

bool ctCombat::Load(pugi::xml_node& load)
{
	bool ret = true;

	return ret;
}

bool ctCombat::Save(pugi::xml_node& save) const
{
	bool ret = true;

	return ret;
}

void ctCombat::OnUITrigger(UIElement* elementTriggered, UI_State ui_state)
{
}

void ctCombat::SetSceneName(string new_scene_name)
{
	scene_name = new_scene_name;
}

uint ctCombat::CalculatedDamage(Entity* attacker, Entity* defender)
{
	return ((attacker->base_stats.base_constitution * 3)*(100 - defender->base_stats.base_physical_defense)) / 100;
}

void ctCombat::SpawnEntities()
{
	int random_number = (rand() % 4);
	// Test assign life with lifebar

	//randomization is ugly, sorry c:
	switch (random_number)
	{
	case 0:
		for (int i = 0; i < App->map->heroes_position_coords.size(); i++)
		{
			switch (i)
			{
			case 0:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, CLERIC);
				break;
			case 1:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, WARRIOR);
				break;
			case 2:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, ELF);
				break;
			case 3:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, DWARF);
				break;
			default:
				break;
			}
		}
		break;
	case 1:
		for (int i = 0; i < App->map->heroes_position_coords.size(); i++)
		{
			switch (i)
			{
			case 0:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, WARRIOR);
				break;
			case 1:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, DWARF);
				break;
			case 2:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, ELF);
				break;
			case 3:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, CLERIC);
				break;
			default:
				break;
			}
		}
		break;
	case 2:
		for (int i = 0; i < App->map->heroes_position_coords.size(); i++)
		{
			switch (i)
			{
			case 0:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, ELF);
				
				break;
			case 1:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, CLERIC);
				break;
			case 2:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, WARRIOR);
				break;
			case 3:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, DWARF);
				break;
			default:
				break;
			}
		}
		break;
	case 3:
		for (int i = 0; i < App->map->heroes_position_coords.size(); i++)
		{
			switch (i)
			{
			case 0:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, CLERIC);
				break;
			case 1:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, WARRIOR);
				break;
			case 2:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, ELF);
				break;
			case 3:
				App->entities->SpawnEntity(App->map->heroes_position_coords.at(i).x, App->map->heroes_position_coords.at(i).y, DWARF);
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}


	for (int i = 0; i < entities_to_spawn.size(); i++)
	{
		switch (entities_to_spawn.at(i))
		{
		case KOBOLD:
			App->entities->SpawnEntity(App->map->enemies_position_coords.at(i).x, App->map->enemies_position_coords.at(i).y, KOBOLD);
			break;
		case GNOLL:
			App->entities->SpawnEntity(App->map->enemies_position_coords.at(i).x, App->map->enemies_position_coords.at(i).y, GNOLL);
			break;
		default:
			break;
		}
	}

}

void ctCombat::LoadDataFromXML()
{
	pugi::xml_document	data_file;
	pugi::xml_node* node = &App->LoadData(data_file);
	node = &node->child("heroes");

	for (pugi::xml_node heroe = node->child("heroe"); heroe; heroe = heroe.next_sibling("heroe"))
	{
		std::string tmp(heroe.attribute("name").as_string());

		if (tmp == "cleric") {
			App->entities->GetCleric()->SetCurrentHealthPoints(heroe.child("values").attribute("health_points").as_uint());
			App->entities->GetCleric()->SetCurrentManaPoints(heroe.child("values").attribute("mana_points").as_uint());
		}
		else if (tmp == "warrior") {
			App->entities->GetWarrior()->SetCurrentHealthPoints(heroe.child("values").attribute("health_points").as_uint());
			App->entities->GetWarrior()->SetCurrentManaPoints(heroe.child("values").attribute("mana_points").as_uint());
		}
		else if (tmp == "dwarf") {
			App->entities->GetDwarf()->SetCurrentHealthPoints(heroe.child("values").attribute("health_points").as_uint());
			App->entities->GetDwarf()->SetCurrentManaPoints(heroe.child("values").attribute("mana_points").as_uint());
		}
		else if (tmp == "elf") {
			App->entities->GetElf()->SetCurrentHealthPoints(heroe.child("values").attribute("health_points").as_uint());
			App->entities->GetElf()->SetCurrentManaPoints(heroe.child("values").attribute("mana_points").as_uint());
		}

	}
}

void ctCombat::SaveDataToXML()
{
	pugi::xml_document	data_file;
	pugi::xml_node* node = &App->LoadData(data_file);
	node = &node->child("heroes");

	for (pugi::xml_node heroe = node->child("heroe"); heroe; heroe = heroe.next_sibling("heroe"))
	{
		std::string tmp(heroe.attribute("name").as_string());

		if (tmp == "cleric") {
			heroe.child("values").attribute("health_points").set_value(App->entities->GetCleric()->GetCurrentHealthPoints());
			heroe.child("values").attribute("mana_points").set_value(App->entities->GetCleric()->GetCurrentManaPoints());
		}
		else if (tmp == "warrior") {
			heroe.child("values").attribute("health_points").set_value(App->entities->GetWarrior()->GetCurrentHealthPoints());
			heroe.child("values").attribute("mana_points").set_value(App->entities->GetWarrior()->GetCurrentManaPoints());
		}
		else if (tmp == "dwarf") {
			heroe.child("values").attribute("health_points").set_value(App->entities->GetDwarf()->GetCurrentHealthPoints());
			heroe.child("values").attribute("mana_points").set_value(App->entities->GetDwarf()->GetCurrentManaPoints());
		}
		else if (tmp == "elf") {
			heroe.child("values").attribute("health_points").set_value(App->entities->GetElf()->GetCurrentHealthPoints());
			heroe.child("values").attribute("mana_points").set_value(App->entities->GetElf()->GetCurrentManaPoints());
		}

	}

	data_file.save_file("data.xml");
	data_file.reset();
}

void ctCombat::SetDataToUI()
{
	// TEST FOR UI BAR WITH CALC DAMAGE
	//Entity* cleric = App->entities->GetCleric();
	//test = (UIBar*)App->gui->AddUIBar(100,100,cleric->base_stats.base_constitution*13,LIFEBAR);

	Entity* cleric = App->entities->GetCleric();
	//cleric_HP_bar = (UIBar*)App->gui->AddUIBar(34, 0, cleric->base_stats.base_constitution * 13, LIFEBAR);
	test = (UIBar*)App->gui->AddUIBar(34, -1, cleric->base_stats.base_constitution * 13, LIFEBAR);
	cleric_mana_bar = (UIBar*)App->gui->AddUIBar(34, 10, cleric->base_stats.base_focus * 13, MANABAR);

	Entity* warrior = App->entities->GetWarrior();
	warrior_HP_bar = (UIBar*)App->gui->AddUIBar(277, -1, warrior->base_stats.base_constitution * 13, LIFEBAR);
	warrior_mana_bar = (UIBar*)App->gui->AddUIBar(277, 10, warrior->base_stats.base_focus * 13, MANABAR);

	Entity* elf = App->entities->GetElf();
	elf_HP_bar = (UIBar*)App->gui->AddUIBar(34, 293, elf->base_stats.base_constitution * 13, LIFEBAR);
	elf_mana_bar = (UIBar*)App->gui->AddUIBar(34, 304, elf->base_stats.base_focus * 13, MANABAR);

	Entity* dwarf = App->entities->GetDwarf();
	dwarf_HP_bar = (UIBar*)App->gui->AddUIBar(277, 293, dwarf->base_stats.base_constitution * 13, LIFEBAR);
	dwarf_mana_bar = (UIBar*)App->gui->AddUIBar(277, 304, dwarf->base_stats.base_focus * 13, MANABAR);
}

void ctCombat::OrderTurnPriority()
{
	bool ordered = false;

	while (!ordered)
	{
		ordered = true;
		std::vector<Entity*>::iterator itnext = turn_priority_entity.begin();
		int count = 0;
		for (std::vector<Entity*>::iterator it = turn_priority_entity.begin(); it != turn_priority_entity.end(); ++it)
		{
			itnext++;
			count++;
			if (count != turn_priority_entity.size())
			{
				if ((*it)->base_stats.base_agility < (*itnext)->base_stats.base_agility)
				{
					Entity* entity_tmp = (*it);

					(*it) = (*itnext);
					it++;
					(*it) = entity_tmp;
					it--;
					ordered = false;
				}
			}
			else
				break;
		}

	}

}

void ctCombat::DrawTurnPriority()
{
	uint x=10, y=30;
	for (std::vector<Entity*>::iterator it = turn_priority_entity.begin(); it != turn_priority_entity.end(); ++it)
	{
		//todo: this should be read from xml or be a define
		SDL_Rect Kobold = { 881,71,26,26 };
		SDL_Rect Gnoll = { 821,8,26,26 };
		SDL_Rect Owlbear = { 1147, 19, 26, 26 };
		SDL_Rect GnollArcher = { 993, 25, 26, 26 };
		SDL_Rect Cleric = { 142, 2652, 26, 26 };
		SDL_Rect Dwarf = { 81, 333, 26, 26 };
		SDL_Rect Elf = { 852, 2150, 26, 26 };
		SDL_Rect Warrior = { 1183, 112, 26, 26 };
		switch ((*it)->type)
		{
		case CLERIC:
			App->render->Blit(App->entities->cleric_spritesheet, x, y, &Cleric);
			break;
		case DWARF:
			App->render->Blit(App->entities->dwarf_spritesheet, x, y, &Dwarf);
			break;
		case ELF:
			App->render->Blit(App->entities->elf_spritesheet, x, y, &Elf);
			break;
		case WARRIOR:
			App->render->Blit(App->entities->warrior_spritesheet, x, y, &Warrior);
			break;
		case KOBOLD:
			App->render->Blit(App->entities->kobold_spritesheet, x, y, &Kobold);
			break;
		case GNOLL:
			App->render->Blit(App->entities->gnoll_spritesheet, x, y, &Gnoll);
			break;
		case GNOLL_ARCHER:
			App->render->Blit(App->entities->gnollArcher_spritesheet, x, y, &GnollArcher);
			break;
		case OWLBEAR:
			App->render->Blit(App->entities->owlbear_spritesheet, x, y, &Elf);
			break;
		case MINIHEROES:
			break;
		case NO_TYPE:
			break;
		default:
			break;
		}
		
		y += 26;

	}

}

bool ctCombat::PerformActionWithEntity(Entity * entity_to_perform_action)
{

	switch (entity_to_perform_action->type)
	{
	case CLERIC:
		break;
	case DWARF:
		break;
	case ELF:
		break;
	case WARRIOR:
		break;
	case KOBOLD: {
		if (IsGoingToDoAnythingClever(entity_to_perform_action)) {
			//in this case the kobold will search the weakest heroe since we dont have abilities
			Entity* weakest_heroe = GetTheWeakestHeroe();
			
		}
		else {
			//in this case, the kobold will attack one random heroe
			Entity* weakest_heroe = GetRandomHeroe();

		}
	}
		break;
	case GNOLL:
		break;
	case GNOLL_ARCHER:
		break;
	case OWLBEAR:
		break;
	case MINIHEROES:
	case NO_TYPE:
		LOG("this should not happen");
		break;
	default:
		break;
	}

	return false;
}

bool ctCombat::IsGoingToDoAnythingClever(Entity * entity)
{

	int random_number = (rand() % 100) + 1; //random del 1-100
	
	return entity->GetCurrentJudgement()<=random_number;
}

Entity * ctCombat::GetTheWeakestHeroe()
{
	//now we get the lowest hp

	Entity* tmp_entity = App->entities->GetCleric();
	if(tmp_entity->GetCurrentHealthPoints() == 0)
		tmp_entity = App->entities->GetDwarf();
	if (tmp_entity->GetCurrentHealthPoints() == 0)
		tmp_entity = App->entities->GetElf();
	if (tmp_entity->GetCurrentHealthPoints() == 0)
		tmp_entity = App->entities->GetWarrior();

	if (tmp_entity->GetCurrentHealthPoints() > App->entities->GetCleric()->GetCurrentHealthPoints() && App->entities->GetCleric()->GetCurrentHealthPoints()>0)
		tmp_entity = App->entities->GetCleric();

	if (tmp_entity->GetCurrentHealthPoints() > App->entities->GetDwarf()->GetCurrentHealthPoints() && App->entities->GetDwarf()->GetCurrentHealthPoints()>0)
		tmp_entity = App->entities->GetDwarf();

	if (tmp_entity->GetCurrentHealthPoints() > App->entities->GetElf()->GetCurrentHealthPoints() && App->entities->GetElf()->GetCurrentHealthPoints()>0)
		tmp_entity = App->entities->GetElf();

	if (tmp_entity->GetCurrentHealthPoints() > App->entities->GetWarrior()->GetCurrentHealthPoints() && App->entities->GetWarrior()->GetCurrentHealthPoints()>0)
		tmp_entity = App->entities->GetWarrior();

	return tmp_entity;
}

Entity * ctCombat::GetRandomHeroe()
{
	return nullptr;
}
