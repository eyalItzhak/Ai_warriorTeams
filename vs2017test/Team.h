#pragma once
#include "Warrior.h"
#include "Luggage.h"

#include <vector>
class Team
{
public:
	Team(int health, int ammo);
	~Team();
	std::vector<Cell*> GetTargetByType(int type);
	void PlayTurn(int teamNum);
	Warrior* warrior1;
	Warrior* warrior2;
	Luggage* luggage;

	int NextRow, NextCol;
	int LuggageNextRow, LuggageNextCol;
	bool luggageMove = false;
	void TeamMemeberTakeDamageEvent(int col_pos, int row_pos, int amount);
	int getWarriorsType();

	//Cell* grandeLocation = nullptr;
	double rowGreandeLocation = 0;
	double colGreandeLocation = 0;

	float DistanceToTeam(Cell* map_pos);
	float FireSolution(Cell* map_pos, int maze[MSZ][MSZ]);
	float getFireSolution(Cell* map_pos, Cell* charecterPos, int maze[MSZ][MSZ]);
	double calculateAngle(int startX, int startY, int endX, int endY);
};

