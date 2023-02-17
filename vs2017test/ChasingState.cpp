#include "ChasingState.h"
#include "RuningState.h"

ChasingState::ChasingState()
{
}

ChasingState::~ChasingState()
{
}

void ChasingState::MakeTransition(Cell* npc)
{
	npc->getCurrentState()->OnStateExit(npc);
	npc->setCurrentState(new RuningState());
	npc->getCurrentState()->OnStateEnter(npc);
}

void ChasingState::OnStateEnter(Cell* npc)
{
	npc->setChasing(true);
}

void ChasingState::OnStateExit(Cell* npc)
{
	npc->setChasing(false);
}
