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
	void PlayTurn();
	Warrior* warrior1;
	Warrior* warrior2;
	Luggage* luggage;
	int NextRow, NextCol;
	int LuggageNextRow, LuggageNextCol;
	bool luggageMove = false;
};

