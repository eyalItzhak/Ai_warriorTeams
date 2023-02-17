#pragma once
#include "Warrior.h"
#include "Luggage.h"
#include <vector>
class Team
{
public:
	Team(int health, int ammo);
	~Team();
	std::vector<Cell*> GetTargetByType(int type);
	Warrior* warrior1;
	Warrior* warrior2;
	Luggage* luggage;
};

