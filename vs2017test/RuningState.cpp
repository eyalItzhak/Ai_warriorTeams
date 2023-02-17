#include "RuningState.h"
#include "ChasingState.h"


RuningState::RuningState()
{
}

RuningState::~RuningState()
{
}

void RuningState::MakeTransition(Cell* npc)
{
	npc->getCurrentState()->OnStateExit(npc);
	npc->setCurrentState(new ChasingState());
	npc->getCurrentState()->OnStateEnter(npc);
}

void RuningState::OnStateEnter(Cell* npc)
{
	npc->setChasing(false);
}

void RuningState::OnStateExit(Cell* npc)
{
	npc->setChasing(true);
}
