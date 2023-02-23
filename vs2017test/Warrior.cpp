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

Warrior::Warrior(int health, int am)
{
	isChasing = true;
	hp = health;
	ammo = am;
	state = new ChasingState();
	badJudgment = 0.1;
}
void Warrior::PlayTurn()
{
	if (ammo < MIN_AMMO)
		needAmmo = true;
	if (isChasing)
	{
		if (hp < MIN_HP)
		{
			needHp = true;
			getState()->MakeTransition(this);
		}
		if (ammo == 0)
		{
			getState()->MakeTransition(this);
		}
	}
	else
	{
		if (hp >= MIN_HP)
		{
			needHp = false;
			getState()->MakeTransition(this);
		}
		if (ammo >= MIN_AMMO)
		{
			needAmmo = false;
			getState()->MakeTransition(this);
		}
	}
	
		
}
Warrior::~Warrior()
{
}