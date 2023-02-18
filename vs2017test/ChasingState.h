#pragma once
#include "State.h"
#include "Warrior.h"
class Warrior;
class ChasingState :
    public State
{
public:
	ChasingState();
	~ChasingState();
	void MakeTransition(Character* npc);
	void OnStateEnter(Character* npc);
	void OnStateExit(Character* npc);
};

