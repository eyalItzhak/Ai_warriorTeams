#include "AssistingState.h"
#include "WanderingState.h"

AssistingState::AssistingState()
{
}

AssistingState::~AssistingState()
{
}

void AssistingState::MakeTransition(Cell* npc)
{
	npc->getCurrentState()->OnStateExit(npc);
	npc->setCurrentState(new WanderingState());
	npc->getCurrentState()->OnStateEnter(npc);
}

void AssistingState::OnStateEnter(Cell* npc)
{
	npc->setChasing(true);
}

void AssistingState::OnStateExit(Cell* npc)
{
	npc->setChasing(false);
}