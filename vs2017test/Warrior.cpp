#include "Warrior.h"


int Warrior::GetTarget(int myTeam)
{
	if (isChasing)
	{
		if (myTeam == 0)
			return WARRIOR_TEAM_2;
		else
			return WARRIOR_TEAM_1;
	}
	if (myTeam == 0)
		return LUGGAGE_TEAM_1;
	return LUGGAGE_TEAM_2;
}

Warrior::Warrior(int health, int ammo)
{
	isChasing = true;
	Character(health, ammo, new ChasingState());
}
Warrior::~Warrior()
{
}