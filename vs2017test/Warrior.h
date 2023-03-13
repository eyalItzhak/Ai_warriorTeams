#pragma once
#include "Character.h"
#include "Enum.h"
#include "ChasingState.h"


class Warrior: public Character
{
public:
	Warrior(int health, int ammo);
	int GetTarget();
	void PlayTurn(int myTeam);
	~Warrior();
	const int MIN_HP = 30;
	const int MIN_AMMO = 50;
	bool needHp = false;
	bool needAmmo = false;
	int target;
	int lastTarget = -1;
};

