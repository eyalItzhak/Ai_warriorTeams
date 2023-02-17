#pragma once
#include "Character.h"
#include "Enum.h"
#include "ChasingState.h"
class Warrior: public Character
{
public:
	Warrior(int health, int ammo);
	int GetTarget(int myTeam);
	~Warrior();
	bool isChasing = false;
};

