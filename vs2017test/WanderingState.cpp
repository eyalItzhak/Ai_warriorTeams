#include "WanderingState.h"
#include "AssistingState.h"

WanderingState::WanderingState()
{
}

WanderingState::~WanderingState()
{
}

void WanderingState::MakeTransition(Cell* npc)
{
	npc->getCurrentState()->OnStateExit(npc);
	npc->setCurrentState(new AssistingState());
	npc->getCurrentState()->OnStateEnter(npc);
}

void WanderingState::OnStateEnter(Cell* npc)
{
	npc->setChasing(true);
}

void WanderingState::OnStateExit(Cell* npc)
{
	npc->setChasing(false);
}