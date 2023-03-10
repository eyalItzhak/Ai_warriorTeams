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
			return;
		}
		if (ammo == 0)
		{
			getState()->MakeTransition(this);
			return;
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
		}
		else
		{
			needHp = false;
		}
		if (ammo < MIN_AMMO)
		{
			target = AMMO;
		}
		else
		{
			needAmmo = false;
		}
		if (!needAmmo && !needHp)
		{
			getState()->MakeTransition(this);
			return;
		}
	}
}
Warrior::~Warrior()
{
}