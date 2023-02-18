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

void Team::PlayTurn()
{
	warrior1->PlayTurn();
	warrior2->PlayTurn();
	luggage->PlayTurn(warrior1,warrior2);
}

Team::~Team()
{
}