#pragma once
#include "State.h"
class RuningState :
	public State
{
public:
	RuningState();
	~RuningState();
	void MakeTransition(Cell* npc);
	void OnStateEnter(Cell* npc);
	void OnStateExit(Cell* npc);
};

