#pragma once
#include "State.h"
class AssistingState :
	public State
{
public:
	AssistingState();
	~AssistingState();
	void MakeTransition(Cell* npc);
	void OnStateEnter(Cell* npc);
	void OnStateExit(Cell* npc);
};