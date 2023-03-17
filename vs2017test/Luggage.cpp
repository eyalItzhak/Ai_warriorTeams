#include "Luggage.h"

bool comp(int a, int b)
{
	return (a < b);
}
Luggage::Luggage(int health, int am)
{
	hp = health;
	ammo = am;
	state = new WanderingState();
	0.2;
}
void Luggage::PlayTurn(Warrior* warrior1, Warrior* warrior2)
{
	if (!warrior1->isChasing || !warrior2->isChasing)
	{
		if (hp > 0)
		{
			getState()->MakeTransition(this);
			if (comp(warrior1->getHp(), warrior2->getHp())) // help warrior1
			{
				if (warrior1->needHp)
					target = warrior1;
			}
			else //help warrior2
			{
				if (warrior2->needHp)
					target = warrior2;
			}
		}
		else if (ammo > 0)
		{
			getState()->MakeTransition(this);
			if (comp(warrior1->getAmmo(), warrior2->getAmmo())) // help warrior1
			{
				if (warrior1->needAmmo)
					target = warrior1;
			}
			else //help warrior2
			{
				if (warrior2->needAmmo)
					target = warrior2;
			}
		}
	}
	else
	{
		target = NULL;
	};
}

bool Luggage::canSupport()
{

	if (currentCoolDown == 0) {
		currentCoolDown = currentCoolDown + 1 ;
		return true;
	}

    currentCoolDown = currentCoolDown + 1;

	if (currentCoolDown >= cooldownRound)
		currentCoolDown = 0;

	
	return false;
}

bool Luggage::canReact()
{
	if (myReaction == 0) {
		myReaction = myReaction + 1;
		return true;
	}

	myReaction = myReaction + 1;

	if (myReaction >= sloweReaction)
		myReaction = 0;


	return false;
}

Luggage::~Luggage()
{
}