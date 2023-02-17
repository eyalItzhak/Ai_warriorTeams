#pragma once
#include "State.h"
class ChasingState :
    public State
{
public:
	ChasingState();
	~ChasingState();
	void MakeTransition(Cell* npc);
	void OnStateEnter(Cell* npc);
	void OnStateExit(Cell* npc);
};

