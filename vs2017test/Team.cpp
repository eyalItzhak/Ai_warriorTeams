#include "Team.h"
#include <vector>
#include "Enum.h"

Team::Team(int health, int ammo)
{
	warrior1 = new Warrior(health, ammo);
	warrior2 = new Warrior(health, ammo);
	luggage = new Luggage(health, ammo);
}
std::vector<Cell*>Team::GetTargetByType(int type)
{
	std::vector<Cell*> locations;
	if (type ==WARRIOR_TEAM_1 || type == WARRIOR_TEAM_2)
	{
		Cell* warrior1Loc = warrior1->getLocation();
		Cell* warrior2Loc = warrior2->getLocation();
		locations.push_back(warrior1Loc);
		locations.push_back(warrior2Loc);
	}
	else
	{
		Cell* luggageLoc = luggage->getLocation();
		locations.push_back(luggageLoc);
	}
	return locations;
}

int calculateManhattanDistance(int X1, int X2, int Y1, int Y2) {
	int distance = abs(X2 - X1) + abs(Y2 - Y1);
	return distance;
}

int Team :: closeRangeToTeamWarriors(int row, int col) {
	int range1 = calculateManhattanDistance(row, warrior1->getLocation()->getRow(), col, warrior1->getLocation()->getCol());
	int range2 = calculateManhattanDistance(row, warrior2->getLocation()->getRow(), col, warrior2->getLocation()->getCol());
	return std::min(range1, range2);
}

void Team::PlayTurn()
{
	warrior1->PlayTurn();
	warrior2->PlayTurn();
	luggage->PlayTurn(warrior1,warrior2);
}

Team::~Team()
{
}