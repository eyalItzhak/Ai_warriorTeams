#pragma once
#include "Character.h"
#include "WanderingState.h"
#include "Warrior.h"
class Luggage:public Character
{
public:
	Luggage(int health, int ammo);
	~Luggage();
	void PlayTurn(Warrior* warrior1, Warrior* warrior2);
	Warrior* target;
	int supplyTarget = HP;
	
};


