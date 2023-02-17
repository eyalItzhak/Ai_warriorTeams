#include "Luggage.h"


Luggage::Luggage(int health, int ammo)
{
	Character(health, ammo, new WanderingState());
}

Luggage::~Luggage()
{
}