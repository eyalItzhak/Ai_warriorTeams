#pragma once
#include "State.h"
#include "Luggage.h"
class AssistingState :
	public State
{
public:
	AssistingState();
	~AssistingState();
	void MakeTransition(Character* npc);
	void OnStateEnter(Character* npc);
	void OnStateExit(Character* npc);
};