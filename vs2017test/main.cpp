#include "glut.h"
#include <time.h>
#include <iostream>
#include "Room.h"
#include <vector>
#include <chrono>
#include <thread> 
#include "RuningState.h"
#include "ChasingState.h"
#include "Team.h"
#include "Enum.h"
using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

#pragma region Global Variables

// Maze global variables
const int MSZ = 100;
int maze[MSZ][MSZ];
bool drawPassages = true;

// Type of frontend elements


//Room global variables
int roomAmount = 0;
vector<Room*> rooms;
Cell target_cell;
vector<Cell*> roomGrays;
vector<Cell*> tempGrays;
vector<Cell*> grays;
int supplyPerRoomAmount = 2;
int startHealth = 100;
int startAmmo = 100;
int supplyBuff = 30;
// Game global variables
Cell Pacman_cell;
vector<Cell*> ammoVector;
vector<Cell*> hpVector;
bool found_Ghost = false;
int NextRow, NextCol;
vector<Team*> teams;
int numberOfTeams = 2;
//Game status global variables
bool startGame = false;
bool findPathForPacFlag = false;
bool ScaredGhostsFlag = false;
int pacManNextMoveTimer = 0;
Cell* pacmanNextCell;
bool flag = true;
#pragma endregion

#pragma region Functions Declarition 

void InitMaze();
void ShowMaze();
float cellsDistance(Cell* dest, Cell* src);
void idle();
void sortedInsert(Cell* toInsert);
void CheckNeighborDistance(Cell* pCurrent, int row, int col);
void ConnectRooms(int from, int to);
void RecoverTempGrays();
void SetUpSupply(int type);
Cell* PlaceItem(int type, int room);
void SetUpTeams();
// Game started functions
void gameIteration();
void AStarSearch(int target, Cell* currentLocation, Team* sourceTeam,Team* targetTeam, float badJudgmentFactor, Character* character);
void PacmanRuningBFS();

#pragma endregion

#pragma region Maze Init, ShowMaze and display

void init()
{
	glClearColor(0.8, 0.7, 0.5, 0);// color of window background

	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	srand(time(0)); // initialize random seed

	InitMaze();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer


	ShowMaze();

	glutSwapBuffers(); // show all
}

void InitMaze()
{
	int i, j, random;
	// setup Frame borders
	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = BORDER_WALL;
		maze[MSZ - 1][i] = BORDER_WALL;
		maze[i][0] = BORDER_WALL;
		maze[i][MSZ - 1] = BORDER_WALL;
	}
	// Setup Rooms
	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (rand() % 10 == roomAmount) // mostly WALLs
			{
				int size = rand() % 30 + 10; // min size of a room is 10 by 10 (max 40x40); 

				// If Room cannot be created make a wall by default
				if (j + size >= MSZ - 1 || i + size >= MSZ - 1)
					maze[i][j] = WALL; // 65%
				else {
					roomAmount++;
					Cell* center = new Cell(i + size / 2, j + size / 2, NULL);
					Room* r1 = new Room(center, i, j, size);
					rooms.push_back(r1);
					for (int k = 0; k < size; k++)
						for (int p = 0; p < size; p++)
						{
							maze[i + k][j + p] = SPACE; // 35%
						}
					i += size;
					j += size;
				}
			}
			else
			{
				maze[i][j] = WALL; // 65%
			}
		}
	// Connect Rooms
	for (int i = 0; i < rooms.size(); i++) // Room to connect
	{
		for (int j = 0; j < rooms.size(); j++) // each room to connect to
		{
			if (i == j)
				continue;
			if (i == 0 || rand() % 2 == 1)
			{
				int row = rooms[j]->getCenter()->getRow();
				int col = rooms[j]->getCenter()->getCol();
				maze[row][col] = TARGET;
				target_cell = Cell(row, col, NULL);
				ConnectRooms(i, j);
				RecoverTempGrays();
			}
		}
	}
	/*maze[rooms[0]->getCenter()->getRow()][rooms[0]->getCenter()->getCol()] = WARRIOR_TEAM_1;
	Pacman_cell = Cell(rooms[0]->getCenter()->getRow(), rooms[0]->getCenter()->getCol(), NULL);
	Pacman_cell.setOldStatus(SPACE);
	for (int i = 1; i < rooms.size() && i <= 3; i++)
	{
		Cell* ghost = new Cell(rooms[i]->getCenter()->getRow(), rooms[i]->getCenter()->getCol(), NULL);
		ghost->setOldStatus(SPACE);
		maze[ghost->getRow()][ghost->getCol()] = LUGGAGE_TEAM_1;
		ghostsVector.push_back(ghost);
	}*/
	SetUpTeams();
	SetUpSupply(AMMO);
	SetUpSupply(HP);
}
void SetUpTeams()
{
	int startRoom;
	int secondStartRoom;
	startRoom = rand() % roomAmount;
	Team* team = new Team(startHealth, startAmmo);
	teams.push_back(team);
	team->warrior1->setLocation(PlaceItem(WARRIOR_TEAM_1, startRoom));
	team->warrior1->getLocation()->setOldStatus(SPACE);
	team->warrior2->setLocation(PlaceItem(WARRIOR_TEAM_1, startRoom));
	team->warrior2->getLocation()->setOldStatus(SPACE);
	team->luggage->setLocation(PlaceItem(LUGGAGE_TEAM_1, startRoom));
	team->luggage->getLocation()->setOldStatus(SPACE);
	do
	{
		secondStartRoom = rand() % roomAmount;
	} while (startRoom== secondStartRoom);
	team = new Team(startHealth, startAmmo);
	teams.push_back(team);
	team->warrior1->setLocation(PlaceItem(WARRIOR_TEAM_2, secondStartRoom));
	team->warrior1->getLocation()->setOldStatus(SPACE);
	team->warrior2->setLocation(PlaceItem(WARRIOR_TEAM_2, secondStartRoom));
	team->warrior2->getLocation()->setOldStatus(SPACE);
	team->luggage->setLocation(PlaceItem(LUGGAGE_TEAM_2, secondStartRoom));
	team->luggage->getLocation()->setOldStatus(SPACE);
}
Cell* PlaceItem(int type, int room)
{
	bool validLocation = false;
	Cell* cell = new Cell();
	while (!validLocation)
	{
		int xPos = rooms[room]->getStart_x() + (rand() % rooms[room]->getSize());
		int yPos = rooms[room]->getStart_y() + (rand() % rooms[room]->getSize());
		if (maze[xPos][yPos] == SPACE) {
			maze[xPos][yPos] = type;
			validLocation = true;
			cell->setRow(xPos);
			cell->setCol(yPos);
		}
	}
	return cell;
}
void SetUpSupply(int type)
{

	for (int i = 0; i < roomAmount; i++)
	{
		int counter = 0;
		while (counter < supplyPerRoomAmount)
		{
			if(type == AMMO)
				ammoVector.push_back(PlaceItem(type, i));
			else if(type == HP)
				hpVector.push_back(PlaceItem(type, i));
			counter++;
		}
	}
}

//Iterate over the maze array and set the color according to it's value
void ShowMaze()
{
	int i, j;
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0, 0, 0);// set color black
				break;
			case SPACE:
				glColor3d(1, 1, 1);// set color white
				break;
			case AMMO:
				glColor3d(0.1, 1, 0.1);// set color blue
				break;
			case HP:
				glColor3d(1, 0.1, 0);// set color blue
				break;
			case TARGET:
				glColor3d(1, 0, 0);// set color red
				break;
			case BLACK:
				glColor3d(0.4, 0.4, 0.4);// set color light gray
				break;
			case GRAY:
				glColor3d(1, 0.7, 0);// set color orange
				break;
			case PATH:
				glColor3d(0.7, 0, 0.7);// purple
				break;
			case BORDER_WALL:
				glColor3d(0, 0, 0);// set color black
				break;
			case WARRIOR_TEAM_1:
				glColor3d(1, 0.7, 0);// set color orange
				break;
			case LUGGAGE_TEAM_1:
				glColor3d(0, 0.1, 1);// set color royal blue
				break;
			case WARRIOR_TEAM_2:
				glColor3d(0.7, 0, 0.7);// purple
				break;
			case LUGGAGE_TEAM_2:
				glColor3d(0.2, 45, 0.7);// some color
				break;
			}
			// now show the cell of maze
			glBegin(GL_POLYGON);
			glVertex2d(j, i); // left-bottom vertex
			glVertex2d(j, i + 1); // left-top vertex
			glVertex2d(j + 1, i + 1); // right-top vertex
			glVertex2d(j + 1, i); // right-bottom vertex
			glEnd();
		}
}

// Restore Room Wall and Paths to their previous state
void RecoverTempGrays()
{
	int size = tempGrays.size();
	for (int i = 0; i < size; i++)
	{
		Cell* pCurrent = *tempGrays.begin();
		tempGrays.erase(tempGrays.begin());
		int row = pCurrent->getRow();
		int col = pCurrent->getCol();
		if (maze[row][col] == PATH)
			maze[row][col] = SPACE;
		else
			maze[row][col] = pCurrent->getOldStatus();
	}
}

// A* and BFS by distance connect room to indexed room 
void ConnectRooms(int from, int to)
{
	Cell* pCurrent;
	roomGrays.clear();
	drawPassages = true;
	roomGrays.push_back(rooms[from]->getCenter());
	tempGrays.push_back(rooms[from]->getCenter()); // temp Grays will be reset to thier old value at the end
	rooms[from]->getCenter()->setOldStatus(SPACE);


	while (maze[rooms[to]->getCenter()->getRow()][rooms[to]->getCenter()->getCol()] == TARGET)
	{
		pCurrent = *roomGrays.begin();
		// 2.1 and remove it from grays
		roomGrays.erase(roomGrays.begin());
		// 2.2 paint it BLACK
		int row, col;
		row = pCurrent->getRow();
		col = pCurrent->getCol();

		maze[row][col] = BLACK;
		// 3 check the neighbors of pCurrent and pick the white one and add them to the end of grays
		// UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET || maze[row + 1][col] == WALL)
			CheckNeighborDistance(pCurrent, row + 1, col);
		// DOWN
		if (drawPassages)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET || maze[row - 1][col] == WALL)
				CheckNeighborDistance(pCurrent, row - 1, col);
		// right		
		if (drawPassages)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET || maze[row][col + 1] == WALL)
				CheckNeighborDistance(pCurrent, row, col + 1);
		// left		
		if (drawPassages)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET || maze[row][col - 1] == WALL)
				CheckNeighborDistance(pCurrent, row, col - 1);
	}
}

// Restore path
void RestorePath(Cell* pc)
{
	while (pc != nullptr)
	{
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

//// Check distance
void CheckNeighborDistance(Cell* pCurrent, int row, int col)
{
	if (maze[row][col] == TARGET) // The algorithm is over
	{
		drawPassages = false;
		maze[row][col] = SPACE;
		cout << "Found path" << endl;
		RestorePath(pCurrent);
	}
	else
	{
		Cell* pc = new Cell(row, col, pCurrent);
		pc->setOldStatus(maze[row][col]);
		tempGrays.push_back(pc);
		maze[row][col] = GRAY;
		sortedInsert(pc);

	}
}

//// Insert a new cell by prioritizing his distance from destination
void sortedInsert(Cell* toInsert)
{
	float distance = cellsDistance(&target_cell, toInsert);
	toInsert->setH(distance);
	toInsert->setG((toInsert->getParent()->getG() + 1));
	toInsert->setF();

	int x = 0;
	for (Cell* c : roomGrays) {
		if (c->getF() >= toInsert->getF())
		{
			roomGrays.insert(roomGrays.begin() + x, toInsert);
			return;
		}
		x++;
	}
	roomGrays.push_back(toInsert);
}

//// Calculate distance between 2 Cells
float cellsDistance(Cell* dest, Cell* src)
{
	// Calculating distance
	double status = src->getOldStatus();
	if (status == SPACE)
		status = 0;
	else
		status = 0.001;

	return sqrt(pow(dest->getCol() - src->getCol(), 2) +
		pow(dest->getRow() - src->getRow(), 2) * (1.0 * status));
}

#pragma endregion

#pragma region Game Started

#pragma region Ghost Logic

#pragma region Ghosts Chasing Logic

int FindCellPosition(vector<Cell*> cells,int row,int col)
{
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells[i]->getCol() == col && cells[i]->getRow() == row) 
		{
			cout << "Found cell position!" << endl;
			return i;
		}
	}
	cout << "Didn't find cell position!" << endl;
	return -1;
}
void RecoverTempGraysGhosts(int source, int target, Team* sourceTeam,Team* targetTeam, Character* charcter)
{
	int size = tempGrays.size();
	for (int i = 0; i < size; i++)
	{
		Cell* pCurrent = *tempGrays.begin();
		tempGrays.erase(tempGrays.begin());
		int row = pCurrent->getRow();
		int col = pCurrent->getCol();
		int nextRow = sourceTeam->luggageMove ? sourceTeam->LuggageNextRow : sourceTeam->NextRow;
		int nextCol = sourceTeam->luggageMove ? sourceTeam->LuggageNextCol : sourceTeam->NextCol;
		if (row == nextRow && col == nextCol) // move the source to the next step
		{
			if (pCurrent->getOldStatus() == source)
				continue;
			maze[charcter->getLocation()->getRow()][charcter->getLocation()->getCol()] = SPACE;
			charcter->getLocation()->setOldStatus(pCurrent->getOldStatus());
			charcter->getLocation()->setCol(col);
			charcter->getLocation()->setRow(row);
			maze[row][col] = source;
			continue;
		}
		else if (maze[row][col] == TARGET)
		{
			if (startGame == 1)
			{
				maze[row][col] = target;
				maze[charcter->getLocation()->getRow()][charcter->getLocation()->getCol()] = source;
			}
				
			continue;
		}
		maze[row][col] = pCurrent->getOldStatus();
	}
}
bool ValidMove(int row, int col)
{
	return maze[row][col] == SPACE || maze[row][col] == AMMO || maze[row][col] == HP || maze[row][col] == GRAY || maze[row][col] == BLACK || maze[row][col] == PATH;
}
void FindWrongMove(Cell* pc, Team* sourceTeam)
{
	int row = pc->getRow();
	int col = pc->getCol();
	bool valid = false;
	do
	{
		if (ValidMove(pc->getRow() + 1, pc->getCol())) //UP
		{
			row = row + 1;
			valid = true;
		}
		else if (ValidMove(pc->getRow(), pc->getCol() + 1)) //RIGHT
		{
			col = col + 1;
			valid = true;
		}
		else if (ValidMove(pc->getRow() - 1, pc->getCol())) //DOWN
		{
			row = row - 1;
			valid = true;
		}
		else if(ValidMove(pc->getRow(), pc->getCol() - 1))//LEFT
		{
			col = col - 1;
			valid = true;
		}
	} while (!valid);
	
	if (sourceTeam->luggageMove) 
	{
		sourceTeam->LuggageNextCol = col;
		sourceTeam->LuggageNextRow = row;
	}
	else
	{
		sourceTeam->NextCol = col;
		sourceTeam->NextRow = row;
	}
	cout << "Bad Move Found!" << endl;
	/*pc->setRow(move->getRow());
	pc->setCol(move->getCol());*/
	
}

void RestorePathGhosts(Cell* pc,Team* sourceTeam, float badJudgmentFactor) //add flag for running and change next row and nextCol logic to fit situation
{
	while (pc->getParent() != nullptr)
	{
		if (pc->getParent()->getParent() == nullptr)
		{
			if (rand() % (int)(1.0 / badJudgmentFactor) == 0) //move to the WRONG direction
			{
				/*cout << "Bad Move!" << endl;
				Cell* move =FindWrongMove(pc, sourceTeam);
				maze[move->getRow()][move->getCol()] = PATH;*/
				//move->setOldStatus(pc.getst)
				/*Cell* tmp = pc->getParent();
				pc->deleteParent();
				pc = tmp;*/
				//continue;
				FindWrongMove(pc, sourceTeam);
			}
			else // move to the right direction
			{
				if (sourceTeam->luggageMove)
				{
					sourceTeam->LuggageNextCol = pc->getCol();
					sourceTeam->LuggageNextRow = pc->getRow();
				}
				else
				{
					sourceTeam->NextCol = pc->getCol();
					sourceTeam->NextRow = pc->getRow();
				}
			}
			
		}
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

//// Check distance
void CheckNeighborDistanceGhosts(Cell* pCurrent, int row, int col, int target, Team* team, float judgment,Character* character)
{
	if (maze[row][col] == target) // The algorithm is over
	{
		drawPassages = false;
		maze[row][col] = TARGET;
		Cell* targetLocation = new Cell(row, col, pCurrent);
		tempGrays.push_back(targetLocation);
		//NEED TO ADD FIGHTING HERE MAYBE?!
		if (pCurrent->getParent() == nullptr) // Ghost one step away from the pacman
		{
			if (target == HP || target == AMMO)
			{
				if (target == HP)
					character->setHp(character->getHp() + supplyBuff); // ADD HP
				else 
					character->setAmmo(character->getAmmo() + supplyBuff); // ADD AMMO
				if (team->luggageMove)
				{
					team->LuggageNextCol = col;
					team->LuggageNextRow = row;
				}
				else
				{
					team->NextCol = col;
					team->NextRow = row;
				}
				maze[row][col] = SPACE;
			}
			
			//cout << "GameOver" << endl;
			//startGame = 0;
			/*NextCol = col;
			NextRow = row;*/
		}
		/*else
			cout << "Found Pacman Path" << endl;*/
		RestorePathGhosts(pCurrent, team, judgment);
	}
	else
	{
		Cell* pc = new Cell(row, col, pCurrent);
		pc->setOldStatus(maze[row][col]);
		tempGrays.push_back(pc);
		maze[row][col] = GRAY;
		sortedInsert(pc);
	}
}
bool FoundTarget(Team* targetTeam, int target)
{
	vector<Cell*> locations;
	if (target == HP)
		locations = hpVector;

	else if (target == AMMO)
		locations = ammoVector;
	else
		locations = targetTeam->GetTargetByType(target);
	for (int i = 0; i < locations.size(); i++)
	{
		if (maze[locations[i]->getRow()][locations[i]->getCol()] != target)
			return false;
	}
	return true;
}

void AStarSearch(int target, Cell* currentLocation,Team* sourceTeam, Team* targetTeam, float judgment, Character* character)
{
	roomGrays.clear();
	drawPassages = true;
	Cell* pCurrent = currentLocation;
	roomGrays.push_back(pCurrent);


	tempGrays.push_back(currentLocation); // temp Grays will be reset to thier old value at the end


	while (FoundTarget(targetTeam, target))
	{
		pCurrent = *roomGrays.begin();
		// 2.1 and remove it from grays
		roomGrays.erase(roomGrays.begin());
		// 2.2 paint it BLACK
		int row, col;
		row = pCurrent->getRow();
		col = pCurrent->getCol();

		maze[row][col] = BLACK;
		// 3 check the neighbors of pCurrent and pick the white one and add them to the end of grays
		// UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == target || (maze[row + 1][col] == AMMO) || (maze[row + 1][col] == HP))
			CheckNeighborDistanceGhosts(pCurrent, row + 1, col, target, sourceTeam, judgment, character);
		// DOWN
		if (drawPassages)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == target || (maze[row - 1][col] == AMMO) || (maze[row - 1][col] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row - 1, col, target, sourceTeam, judgment, character);
		// right		
		if (drawPassages)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == target || (maze[row][col + 1] == AMMO) || (maze[row][col + 1] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row, col + 1, target, sourceTeam, judgment, character);
		// left		
		if (drawPassages)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == target || (maze[row][col - 1] == AMMO) || (maze[row][col - 1] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row, col - 1, target, sourceTeam, judgment, character);
	}
}
#pragma endregion

#pragma region Ghost Runing Logic

//void MoveGhost(int GhostIndex)
//{
//	Cell* pc = pacmanNextCell;
//	while (pc->getParent() != nullptr)
//	{
//		if (pc->getParent()->getParent() == nullptr)
//		{
//			NextCol = pc->getCol();
//			NextRow = pc->getRow();
//			pc->deleteParent();
//			break;
//		}
//		pc = pc->getParent();
//	}
//
//	maze[ghostsVector[GhostIndex]->getRow()][ghostsVector[GhostIndex]->getCol()] = ghostsVector[GhostIndex]->getOldStatus();
//	Pacman_cell.setCol(NextCol);
//	Pacman_cell.setRow(NextRow);
//	ghostsVector[GhostIndex]->setOldStatus(maze[NextRow][NextCol]);
//	maze[NextRow][NextCol] = LUGGAGE_TEAM_1;
//}
//
//void RecoverTempGhostRuning(int GhostIndex)
//{
//	int size = tempGrays.size();
//	for (int i = 0; i < size; i++)
//	{
//		Cell* pCurrent = *tempGrays.begin();
//		tempGrays.erase(tempGrays.begin());
//		int row = pCurrent->getRow();
//		int col = pCurrent->getCol();
//		if (row == NextRow && col == NextCol)
//		{
//			ghostsVector[GhostIndex]->setCol(NextCol);
//			ghostsVector[GhostIndex]->setRow(NextRow);
//			ghostsVector[GhostIndex]->setOldStatus(pCurrent->getOldStatus());
//			maze[NextRow][NextCol] = LUGGAGE_TEAM_1;
//			continue;
//		}
//		maze[row][col] = pCurrent->getOldStatus();
//	}
//}
//
//void RestorePathGhostRuning(Cell* pc)
//{
//	pacmanNextCell = pc;
//	while (pc->getParent() != nullptr)
//	{
//		if (pc->getParent()->getParent() == nullptr)
//		{
//			NextCol = pc->getCol();
//			NextRow = pc->getRow();
//			maze[pc->getRow()][pc->getCol()] = PATH;
//			Cell* tmp = pc->getParent();
//			pc->deleteParent();
//			pc = tmp;
//			continue;
//		}
//		maze[pc->getRow()][pc->getCol()] = PATH;
//		pc = pc->getParent();
//	}
//
//}
//
//int checkDistanceFromPacman(Cell* pc, int gRow, int gCol)
//{
//	return sqrt(pow(pc->getCol() - gCol, 2) +
//		pow(pc->getRow() - gRow, 2));
//}
//
//void checkNeighborGhostRuning(int row, int col, Cell* pCurrent)
//{
//	Cell* pneig;
//	if (maze[row][col] == WARRIOR_TEAM_1)
//	{
//		int max = 0;
//		int size = grays.size();
//		for (int i = 0; i < size; i++)
//		{
//			Cell* cell = *grays.begin();
//			grays.erase(grays.begin());
//			int value = checkDistanceFromPacman(cell, row, col);
//			if (max < value)
//			{
//				max = value;
//				pCurrent = cell;
//			}
//		}
//		cout << "Pacman Found, Runing Away" << endl;
//		drawPassages = false;
//		RestorePathGhostRuning(pCurrent);
//	}
//	else //this is white 
//	{
//		pneig = new Cell(row, col, pCurrent);//abd pcurrent as parent
//		tempGrays.push_back(pneig);
//		pneig->setOldStatus(maze[row][col]);
//		maze[row][col] = GRAY;
//		grays.push_back(pneig);
//	}
//}
//
//void GhostRuningBFS(int GhostIndex)
//{
//	grays.clear();
//	drawPassages = true;
//	Cell* pCurrent = ghostsVector[GhostIndex];
//	grays.push_back(pCurrent);
//	findPathForPacFlag = false;
//	tempGrays.push_back(ghostsVector[GhostIndex]); // temp Grays will be reset to thier old value at the end
//	Pacman_cell.setOldStatus(SPACE);
//	int r, c;
//	while (!grays.empty() && drawPassages == true)
//	{
//		pCurrent = *grays.begin();
//		// remove pCurr from grays and paint it black
//		grays.erase(grays.begin());
//		r = pCurrent->getRow();
//		c = pCurrent->getCol();
//		maze[r][c] = BLACK;
//		//add non-visited neighbors
//		// up
//		if (maze[r + 1][c] == SPACE || maze[r + 1][c] == AMMO || maze[r + 1][c] == WARRIOR_TEAM_1)
//		{
//			checkNeighborGhostRuning(r + 1, c, pCurrent);
//		}
//		// down
//		if (drawPassages && maze[r - 1][c] == SPACE || maze[r - 1][c] == AMMO || maze[r - 1][c] == WARRIOR_TEAM_1)
//		{
//			checkNeighborGhostRuning(r - 1, c, pCurrent);
//		}
//		// left 
//		if (drawPassages && maze[r][c - 1] == SPACE || maze[r][c - 1] == AMMO || maze[r][c - 1] == WARRIOR_TEAM_1)
//		{
//			checkNeighborGhostRuning(r, c - 1, pCurrent);
//		}
//		// right
//		if (drawPassages && maze[r][c + 1] == SPACE || maze[r][c + 1] == AMMO || maze[r][c + 1] == WARRIOR_TEAM_1)
//		{
//			checkNeighborGhostRuning(r, c + 1, pCurrent);
//		}
//	}
//}

#pragma endregion

#pragma endregion

//#pragma region Pacman Logic
//
//#pragma region Pacman is Chasing Logic
//
//void RecoverTempPacmanChasing()
//{
//	int size = tempGrays.size();
//	bool ghostUpdated = false;
//	int pacmanRow = Pacman_cell.getRow();
//	int pacmanCol = Pacman_cell.getCol();
//	for (int i = 0; i < size; i++)
//	{
//		Cell* pCurrent = *tempGrays.begin();
//		tempGrays.erase(tempGrays.begin());
//		int row = pCurrent->getRow();
//		int col = pCurrent->getCol();
//		if (row == NextRow && col == NextCol)
//		{
//			Pacman_cell.setOldStatus(SPACE);
//			Pacman_cell.setCol(NextCol);
//			Pacman_cell.setRow(NextRow);
//			maze[NextRow][NextCol] = WARRIOR_TEAM_1;
//			continue;
//		}
//		maze[row][col] = pCurrent->getOldStatus();
//	}
//}
//
//void RestorePathPacman(Cell* pc) {
//
//	while (pc->getParent() != nullptr)
//	{
//		if (pc->getParent()->getParent() == nullptr)// Check if last iteration
//		{
//			NextRow = pc->getRow();
//			NextCol = pc->getCol();
//		}
//		maze[pc->getRow()][pc->getCol()] = PATH;
//		pc = pc->getParent();
//	}
//}
//
//void ResetGhostLocation()
//{
//	for (int i = 0; i < ghostsVector.size(); i < i++)
//	{
//		if (ghostsVector[i]->getRow() == NextRow && ghostsVector[i]->getCol() == NextCol) // This is the Ghost who was eaten
//		{
//			ghostsVector[i]->setRow(rooms[i + 1]->getCenter()->getRow());
//			ghostsVector[i]->setCol(rooms[i + 1]->getCenter()->getCol());
//			maze[Pacman_cell.getRow()][Pacman_cell.getCol()] = SPACE;
//			Pacman_cell.setOldStatus(SPACE);
//			Pacman_cell.setCol(NextCol);
//			Pacman_cell.setRow(NextRow);
//			maze[NextRow][NextCol] = WARRIOR_TEAM_1;
//			break;
//		}
//	}
//}
//
//void checkNeighborPacmanChasing(int row, int col, Cell* pCurrent)
//{
//	Cell* pneig;
//	if (maze[row][col] == LUGGAGE_TEAM_1)
//	{
//		drawPassages = false;
//		if (pCurrent->getParent() == nullptr) // Pacman is one step away from a LUGGAGE_TEAM_1
//		{
//			NextCol = col;
//			NextRow = row;
//			//Reset Ghost location
//			ResetGhostLocation();
//			cout << "Ghost Dead Will Respawn" << endl;
//		}
//		else
//		{
//			cout << "Ghost Found" << endl;
//			RestorePathPacman(pCurrent);
//		}
//
//	}
//	else //this is white 
//	{
//		pneig = new Cell(row, col, pCurrent);//abd pcurrent as parent
//		tempGrays.push_back(pneig);
//		pneig->setOldStatus(maze[row][col]);
//		maze[row][col] = GRAY;
//		grays.push_back(pneig);
//	}
//}
//
//void PacmanChasingBFS()
//{
//	grays.clear();
//	drawPassages = true;
//	Cell* pCurrent = &Pacman_cell;
//	grays.push_back(pCurrent);
//	findPathForPacFlag = false;
//	tempGrays.push_back(&Pacman_cell); // temp Grays will be reset to thier old value at the end
//	int r, c;
//	while (!grays.empty() && drawPassages == true)
//	{
//		pCurrent = *grays.begin();
//		// remove pCurr from grays and paint it black
//		grays.erase(grays.begin());
//		r = pCurrent->getRow();
//		c = pCurrent->getCol();
//		maze[r][c] = BLACK;
//		//add non-visited neighbors
//		// up
//		if (maze[r + 1][c] == SPACE || maze[r + 1][c] == LUGGAGE_TEAM_1 || maze[r + 1][c] == AMMO)
//		{
//			checkNeighborPacmanChasing(r + 1, c, pCurrent);
//		}
//		// down
//		if (drawPassages && maze[r - 1][c] == SPACE || maze[r - 1][c] == LUGGAGE_TEAM_1 || maze[r - 1][c] == AMMO)
//		{
//			checkNeighborPacmanChasing(r - 1, c, pCurrent);
//		}
//		// left 
//		if (drawPassages && maze[r][c - 1] == SPACE || maze[r][c - 1] == LUGGAGE_TEAM_1 || maze[r][c - 1] == AMMO)
//		{
//			checkNeighborPacmanChasing(r, c - 1, pCurrent);
//		}
//		// right
//		if (drawPassages && maze[r][c + 1] == SPACE || maze[r][c + 1] == LUGGAGE_TEAM_1 || maze[r][c + 1] == AMMO)
//		{
//			checkNeighborPacmanChasing(r, c + 1, pCurrent);
//		}
//	}
//}
//
//#pragma endregion
//
//#pragma region Pacman is Runing Logic
//
//void MovePacman()
//{
//	Cell* pc = pacmanNextCell;
//	while (pc->getParent() != nullptr)
//	{
//		if (pc->getParent()->getParent() == nullptr)
//		{
//			NextCol = pc->getCol();
//			NextRow = pc->getRow();
//			pc->deleteParent();
//			break;
//		}
//		pc = pc->getParent();
//	}
//
//	maze[Pacman_cell.getRow()][Pacman_cell.getCol()] = SPACE;
//	Pacman_cell.setCol(NextCol);
//	Pacman_cell.setRow(NextRow);
//	maze[NextRow][NextCol] = WARRIOR_TEAM_1;
//}
//
//void RecoverTempPacmanRuning()
//{
//	int size = tempGrays.size();
//	int pacmanRow = Pacman_cell.getRow();
//	int pacmanCol = Pacman_cell.getCol();
//	for (int i = 0; i < size; i++)
//	{
//		Cell* pCurrent = *tempGrays.begin();
//		tempGrays.erase(tempGrays.begin());
//		int row = pCurrent->getRow();
//		int col = pCurrent->getCol();
//		if (row == NextRow && col == NextCol)
//		{
//			Pacman_cell.setCol(NextCol);
//			Pacman_cell.setRow(NextRow);
//			maze[NextRow][NextCol] = WARRIOR_TEAM_1;
//			continue;
//		}
//		maze[row][col] = pCurrent->getOldStatus();
//	}
//}
//
//void RestorePathPacmanRuning(Cell* pc)
//{
//	pacmanNextCell = pc;
//	while (pc->getParent() != nullptr)
//	{
//		if (pc->getParent()->getParent() == nullptr)
//		{
//			NextCol = pc->getCol();
//			NextRow = pc->getRow();
//			maze[pc->getRow()][pc->getCol()] = PATH;
//			Cell* tmp = pc->getParent();
//			pc->deleteParent();
//			pc = tmp;
//			continue;
//		}
//		maze[pc->getRow()][pc->getCol()] = PATH;
//		pc = pc->getParent();
//	}
//
//}
//
//int checkDistanceFromGhost(Cell* pc, int gRow, int gCol)
//{
//	double status = pc->getOldStatus();
//	if (status == AMMO)
//		status = 0.4;
//	else if (status == SPACE)
//		status = 0.1;
//	return sqrt(pow(pc->getCol() - gCol, 2) +
//		pow(pc->getRow() - gRow, 2) * (1.0 * status));
//}
//
//void checkNeighborPacmanRuning(int row, int col, Cell* pCurrent)
//{
//	Cell* pneig;
//	if (maze[row][col] == LUGGAGE_TEAM_1)
//	{
//		int max = 0;
//		int size = grays.size();
//		for (int i = 0; i < size; i++)
//		{
//			Cell* cell = *grays.begin();
//			grays.erase(grays.begin());
//			int value = checkDistanceFromGhost(cell, row, col);
//			if (max < value)
//			{
//				max = value;
//				pCurrent = cell;
//			}
//		}
//		cout << "Ghost Found" << endl;
//		drawPassages = false;
//		RestorePathPacmanRuning(pCurrent);
//	}
//	else //this is white 
//	{
//		pneig = new Cell(row, col, pCurrent);//abd pcurrent as parent
//		tempGrays.push_back(pneig);
//		pneig->setOldStatus(maze[row][col]);
//		maze[row][col] = GRAY;
//		grays.push_back(pneig);
//	}
//}
//
//void PacmanRuningBFS()
//{
//	grays.clear();
//	drawPassages = true;
//	Cell* pCurrent = &Pacman_cell;
//	grays.push_back(pCurrent);
//	findPathForPacFlag = false;
//	tempGrays.push_back(&Pacman_cell); // temp Grays will be reset to thier old value at the end
//	Pacman_cell.setOldStatus(SPACE);
//	int r, c;
//	while (!grays.empty() && drawPassages == true)
//	{
//		pCurrent = *grays.begin();
//		// remove pCurr from grays and paint it black
//		grays.erase(grays.begin());
//		r = pCurrent->getRow();
//		c = pCurrent->getCol();
//		maze[r][c] = BLACK;
//		//add non-visited neighbors
//		// up
//		if (maze[r + 1][c] == SPACE || maze[r + 1][c] == AMMO || maze[r + 1][c] == LUGGAGE_TEAM_1)
//		{
//			checkNeighborPacmanRuning(r + 1, c, pCurrent);
//		}
//		// down
//		if (drawPassages && maze[r - 1][c] == SPACE || maze[r - 1][c] == AMMO || maze[r - 1][c] == LUGGAGE_TEAM_1)
//		{
//			checkNeighborPacmanRuning(r - 1, c, pCurrent);
//		}
//		// left 
//		if (drawPassages && maze[r][c - 1] == SPACE || maze[r][c - 1] == AMMO || maze[r][c - 1] == LUGGAGE_TEAM_1)
//		{
//			checkNeighborPacmanRuning(r, c - 1, pCurrent);
//		}
//		// right
//		if (drawPassages && maze[r][c + 1] == SPACE || maze[r][c + 1] == AMMO || maze[r][c + 1] == LUGGAGE_TEAM_1)
//		{
//			checkNeighborPacmanRuning(r, c + 1, pCurrent);
//		}
//	}
//}
//
//#pragma endregion
//
//#pragma endregion
void MoveTeams(int teamNum, int enemyTeam)
{
	teams[teamNum]->PlayTurn(teamNum);
	//warrior 1
	int target = teams[teamNum]->warrior1->GetTarget();
 	AStarSearch(target, teams[teamNum]->warrior1->getLocation(), teams[teamNum],teams[enemyTeam], teams[teamNum]->warrior1->getBadJudgment(), teams[teamNum]->warrior1);
 	RecoverTempGraysGhosts(WARRIOR_TEAM_1+ teamNum, target, teams[teamNum],teams[enemyTeam], teams[teamNum]->warrior1);
	//warrior 2
	target = teams[teamNum]->warrior2->GetTarget();
	AStarSearch(target, teams[teamNum]->warrior2->getLocation(), teams[teamNum],teams[enemyTeam], teams[teamNum]->warrior2->getBadJudgment(), teams[teamNum]->warrior2);
  	RecoverTempGraysGhosts(WARRIOR_TEAM_1 + teamNum, target, teams[teamNum],teams[enemyTeam], teams[teamNum]->warrior2);
	////luggage 
	/*teams[teamNum]->luggageMove = true;
	target = WARRIOR_TEAM_1 + teamNum;
	AStarSearch(target, teams[teamNum]->luggage->getLocation(),teams[teamNum], teams[teamNum]);
	RecoverTempGraysGhosts(LUGGAGE_TEAM_1 + teamNum, target, teams[teamNum],teams[teamNum], *teams[teamNum]->luggage);*/
}
void gameIteration()
{
	if (startGame == 1)
	{
		if (flag)
		{
			
			teams[0]->warrior1->setHp(10);
			flag = false;
		}
		printf("Warrior 1 mode: %d\n", teams[0]->warrior1->isChasing);
		MoveTeams(0, 1);
		MoveTeams(1, 0);
		
		
		
		

				/*if (ghostsVector[i]->Execute())
				{
					GhostAStar(i);
					RecoverTempGraysGhosts(i);
				}
				else
				{
					if (pacManNextMoveTimer == 0)
					{
						GhostRuningBFS(i);
						RecoverTempGhostRuning(i);
					}
					else {
						if (pacmanNextCell->getParent() == nullptr)
						{
							GhostRuningBFS(i);
							RecoverTempGhostRuning(i);
							pacManNextMoveTimer = 1;
						}
						else
							MoveGhost(i);
					}
				}*/
		sleep_for(milliseconds(50));
	}
	else
		return;
}

#pragma endregion

#pragma region Idle Menu and Main
// runs all the time in the background
void idle()
{

	if (startGame == 1)
	{
		gameIteration();
	}
	glutPostRedisplay(); // indirect call to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // run Best First Search
		startGame = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 900);
	glutInitWindowPosition(400, 200);
	glutCreateWindow("First Example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutCreateMenu(menu); // defines function menu as "menu" function
	glutAddMenuEntry("Start Game", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // attach to right mouse button

	init();
	glutMainLoop();
}

#pragma endregion
