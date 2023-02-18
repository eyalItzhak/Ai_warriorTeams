#pragma once
#include "State.h"
#include "Luggage.h"
class Luggage;
class WanderingState :
	public State
{
public:
	WanderingState();
	~WanderingState();
	void MakeTransition(Character* npc);
	void OnStateEnter(Character* npc);
	void OnStateExit(Character* npc);
};