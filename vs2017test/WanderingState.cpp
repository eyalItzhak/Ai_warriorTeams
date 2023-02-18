#include "WanderingState.h"
#include "AssistingState.h"

WanderingState::WanderingState()
{
}

WanderingState::~WanderingState()
{
}

void WanderingState::MakeTransition(Character* luggage)
{
	luggage->getState()->OnStateExit(luggage);
	luggage->setState(new AssistingState());
	luggage->getState()->OnStateEnter(luggage);
}

void WanderingState::OnStateEnter(Character* luggage)
{
	luggage->isAssisting = true;
}

void WanderingState::OnStateExit(Character* luggage)
{
	luggage->isAssisting = true;
}