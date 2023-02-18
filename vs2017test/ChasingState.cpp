#include "ChasingState.h"
#include "RuningState.h"
#include "Warrior.h"
ChasingState::ChasingState()
{
}

ChasingState::~ChasingState()
{
}

void ChasingState::MakeTransition(Character* warrior)
{
	warrior->getState()->OnStateExit(warrior);
	warrior->setState(new RuningState());
	warrior->getState()->OnStateEnter(warrior);
}

void ChasingState::OnStateEnter(Character* warrior)
{
	warrior->isChasing = true;
}

void ChasingState::OnStateExit(Character* warrior)
{
	warrior->isChasing = false;
}
