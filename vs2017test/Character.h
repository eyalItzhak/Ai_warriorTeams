#pragma once
#include "State.h"
#include "Enum.h"
class Character
{
protected:
	int hp;
	int ammo;
	Cell* location;
	State* state;
	int type;

public:
	Character();
	Character(int h, int a, State* state);
	int getHp();
	Cell* getLocation() 
	{
		return location;
	}
	State* getState()
	{
		return state;
	}
	void setLocation(Cell* location);
	int getAmmo();
	int getType();
	void setHp(int h);
	void setAmmo(int a);
	~Character();
};