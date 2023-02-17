#pragma once
#include "Cell.h"
//Interface class
class Cell;
class State
{
public:
	State();
	~State();

	virtual void MakeTransition(Cell* npc) = 0;
	virtual void OnStateEnter(Cell* npc) = 0;
	virtual void OnStateExit(Cell* npc) = 0;

};

