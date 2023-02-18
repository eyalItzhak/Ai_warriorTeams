#include "RuningState.h"
#include "ChasingState.h"
#include "Warrior.h"

RuningState::RuningState()
{
}

RuningState::~RuningState()
{
}

void RuningState::MakeTransition(Character* warrior)
{
	warrior->getState()->OnStateExit(warrior);
	warrior->setState(new ChasingState());
	warrior->getState()->OnStateEnter(warrior);
}

void RuningState::OnStateEnter(Character* warrior)
{
	warrior->isChasing = false;
}

void RuningState::OnStateExit(Character* warrior)
{
	warrior->isChasing = true;
}
