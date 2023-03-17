#include "Team.h"
#include <vector>
#include "Enum.h"
#include <cmath>

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

void Team::TeamMemeberTakeDamageEvent(int col_pos,int row_pos, int amount) {
	Cell * CharcterPos = warrior1->getLocation();

	if (CharcterPos->getCol() == col_pos && CharcterPos->getRow() == row_pos) {
		warrior1->takingDamage(amount);
		return;
	}

	CharcterPos = warrior2->getLocation();

	if (CharcterPos->getCol() == col_pos && CharcterPos->getRow() == row_pos) {
		warrior2->takingDamage(amount);
		return;
	}

}

int Team::getWarriorsType()
{
	return warrior1->getType();
}


double average(int a, int b) {
	return (a + b) / 2;
}

float Team::DistanceToTeam(Cell* map_pos)
{
	Cell* warrior_1 = warrior1->getLocation();
	Cell* warrior_2 = warrior2->getLocation();

	// Compute distance between warrior_1 and map_pos
	float distance_1 = std::sqrt(std::pow(warrior_1->getCol() - map_pos->getCol(), 2) +
		std::pow(warrior_1->getRow() - map_pos->getRow(), 2));

	// Compute distance between warrior_2 and map_pos
	float distance_2 = std::sqrt(std::pow(warrior_2->getCol() - map_pos->getCol(), 2) +
		std::pow(warrior_2->getRow() - map_pos->getRow(), 2));

	// Return the minimum distance
	// 

	if (distance_1 <  distance_2) {
		/*grandeLocation = new Cell(average(warrior_1->getRow()+ map_pos->getRow(), average(warrior_1->getCol() + map_pos->getCol()));*/
		rowGreandeLocation = average(warrior_1->getRow() ,map_pos->getRow());
		colGreandeLocation = average(warrior_1->getCol() , map_pos->getCol());
		return distance_1;
	}
	else {
		rowGreandeLocation = average(warrior_2->getRow(), map_pos->getRow());
		colGreandeLocation = average(warrior_2->getCol(), map_pos->getCol());
		return distance_2;
	}
	


	//return std::min(distance_1, distance_2);
}

float Team::FireSolution(Cell* map_pos , int maze[MSZ][MSZ])
{
	Cell* warrior_1 = warrior1->getLocation();
	Cell* warrior_2 = warrior2->getLocation();

	int fireSolution_1 = getFireSolution(map_pos, warrior_1, maze);
	if (fireSolution_1 != -1) {
		return fireSolution_1;
	}
	int fireSolution_2 = getFireSolution(map_pos, warrior_2, maze);
	if (fireSolution_2 != -1) {
		return fireSolution_2;
	}
	return -1;
 }



float Team::getFireSolution(Cell* map_pos, Cell* charecterPos, int maze[MSZ][MSZ]){
	//cheack row axis
	
	int startRow = std::min(map_pos->getRow(), charecterPos->getRow());
	int endRow = std::max(map_pos->getRow(), charecterPos->getRow());

	int startCol = std::min(map_pos->getCol(), charecterPos->getCol());
	int endCol = std::max(map_pos->getCol(), charecterPos->getCol());

	for (int row = startRow; row < endRow; row++)
	{
		if (maze[row][map_pos->getCol()] == WALL) {
			return -1;
		}
		if (maze[row][charecterPos->getCol()] == WALL) {
			return -1;
		}
	}

	

	for (int col = startCol; col < endCol; col++)
	{
		if (maze[map_pos->getRow()][col] == WALL) {
			return -1;
		}
		if (maze[charecterPos->getRow()][col] == WALL) {
			return -1;
		}
	}

	//check center = 
	int row = ((startRow + endRow) / 2);
	int col = ((startCol + endCol) / 2);

	if (maze[row][col] == WALL) {
		return -1;
	}

	return calculateAngle(map_pos->getRow(),map_pos->getCol() , charecterPos->getRow() , charecterPos->getCol());
}


double Team::calculateAngle(int startX, int startY, int endX, int endY) {
	double deltaX = static_cast<double>(endX - startX);
	double deltaY = static_cast<double>(endY - startY);

	double angleRadians = atan2(deltaY, deltaX);
	double angleDegrees = angleRadians * 180.0 / PI;

	// Ensure that the angle is between 0 and 360 degrees
	if (angleDegrees < 0.0) {
		angleDegrees += 360.0;
	}

	return angleDegrees;
}



void Team::PlayTurn(int teamNum)
{
	luggageMove = false;
	warrior1->PlayTurn(teamNum);
	warrior2->PlayTurn(teamNum);
	luggage->PlayTurn(warrior1,warrior2);
}

Team::~Team()
{
}