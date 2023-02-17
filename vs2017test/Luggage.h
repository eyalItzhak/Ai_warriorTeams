#pragma once
#include "Character.h"
#include "WanderingState.h"
class Luggage:public Character
{
public:
	Luggage(int health, int ammo);
	~Luggage();
};


