#include "Warrior.h"


int Warrior::GetTarget()
{
	return target;
}

Warrior::Warrior(int health, int am)
{
	isChasing = true;
	hp = health;
	ammo = am;
	state = new ChasingState();
	badJudgment = 0.1;
}
void Warrior::PlayTurn(int myTeam)
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
		if (myTeam == 0)
			target = WARRIOR_TEAM_2;
		else
			target = WARRIOR_TEAM_1;
	}
	else
	{
		if (hp < MIN_HP)
		{
			target = HP;
			return;
		}
			
		if (ammo < MIN_AMMO)
		{
			target = AMMO;
			return;
		}
		if (hp >= MIN_HP)
		{
			needHp = false;
			getState()->MakeTransition(this);
			return;
		}
		if (ammo >= MIN_AMMO)
		{
			needAmmo = false;
			getState()->MakeTransition(this);
			return;
		}
	}
}
Warrior::~Warrior()
{
}