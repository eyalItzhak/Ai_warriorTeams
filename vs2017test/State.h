#pragma once
#include "Character.h"
#include "Cell.h"
//Interface class
class Cell;
class Character;
class State
{
public:
	State();
	~State();

	virtual void MakeTransition(Character* npc) =0;
	virtual void OnStateEnter(Character* npc) =0;
	virtual void OnStateExit(Character* npc) =0;

};

