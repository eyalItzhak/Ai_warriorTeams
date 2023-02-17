#include "Cell.h"
#include "State.h"
using namespace std;
#include <iostream>

static const int ENDTIME = 75;
static const int PACMAN_CHASING_START_TIMER = 25;
static const int STARTTIME = 0;

Cell::Cell()
{
}

Cell::Cell(int r, int c, Cell* p)
{
	row = r;
	col = c;
	parent = p;
	h = 0; // distance
	g = 0; // gravity
	f = 0; // total weight h+g
	timer = 0;
}


Cell::~Cell()
{
}

/// <summary>
/// return true if in chase otherwise return false, and upticks the timer
/// </summary>
/// <returns></returns>
bool Cell::Execute()
{
	if (timer > ENDTIME)
	{
		switch (is_pacman)
		{
		case true:
			if (chasing == false) // meaning pacman will become the chaser now
			{
				timer = PACMAN_CHASING_START_TIMER; // ResetTimer to half the time for pacman as the chaser
				cout << "Pacman is now Chasing" << endl;
			}
			else // Resest Pacman to runing and ghosts to chase
			{
				timer = STARTTIME;
				cout << "Pacman is now Runing" << endl;
			}
			break;

		case false:
			if (chasing == true) // meaning pacman will become the chaser now
			{
				timer = PACMAN_CHASING_START_TIMER; // ResetTimer to half the time for pacman as the chaser
				cout << "Ghost is now Runing" << endl;
			}
			else // Resest Pacman to runing and ghosts to chase
			{
				timer = STARTTIME;
				cout << "Ghost is now Chasing" << endl;
			}
			break;
		}
		currentState->MakeTransition(this);

	}
	timer++;
	return chasing;

}


