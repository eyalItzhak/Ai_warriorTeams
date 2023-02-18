#include "AssistingState.h"
#include "WanderingState.h"
AssistingState::AssistingState()
{
}

AssistingState::~AssistingState()
{
}

void AssistingState::MakeTransition(Character* luggage)
{
	luggage->getState()->OnStateExit(luggage);
	luggage->setState(new WanderingState());
	luggage->getState()->OnStateEnter(luggage);
}

void AssistingState::OnStateEnter(Character* luggage)
{
	luggage->isAssisting = true;
}

void AssistingState::OnStateExit(Character* luggage)
{
	luggage->isAssisting = false;
}