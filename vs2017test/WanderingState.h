#pragma once
#include "State.h"
class WanderingState :
	public State
{
public:
	WanderingState();
	~WanderingState();
	void MakeTransition(Cell* npc);
	void OnStateEnter(Cell* npc);
	void OnStateExit(Cell* npc);
};