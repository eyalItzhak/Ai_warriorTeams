#include "Warrior.h"


int Warrior::GetTarget()
{
	return target;
	/*if (isChasing)
	{
		if (myTeam == 0)
			return WARRIOR_TEAM_2;
		else
			return WARRIOR_TEAM_1;
	}
	if (myTeam == 0)
		return LUGGAGE_TEAM_1;
	return LUGGAGE_TEAM_2;*/
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
		
		/*if (myTeam == 0)
			target = LUGGAGE_TEAM_1;
		else
			target = LUGGAGE_TEAM_2;*/
	}
	
		
}
Warrior::~Warrior()
{
}