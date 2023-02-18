#pragma once
#include "State.h"
#include "Warrior.h"
class RuningState :
	public State
{
public:
	RuningState();
	~RuningState();
	void MakeTransition(Character* npc);
	void OnStateEnter(Character* npc);
	void OnStateExit(Character* npc);
};

