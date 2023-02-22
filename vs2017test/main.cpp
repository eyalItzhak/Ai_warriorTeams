#pragma region includes

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
#include "bullet.h"
#include "Grenade.h"
using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
#pragma endregion
#pragma region globalAndParams
// Maze global variables
int maze[MSZ][MSZ];
bool drawPassages = true;

// Type of frontend elements
const int W = 700; // window Width
const int H = 700; // window Height

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
// Game global variables
Cell Pacman_cell;
vector<Cell*> ghostsVector;
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

bool fireBulet = false, throwGrenade = false;
Bullet* pb = nullptr;
Grenade* pg = nullptr;

double security_map[MSZ][MSZ] = { 0 };

#pragma endregion
#pragma region declration
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
void gameIteration();
void AStarSearch(int target, Cell* currentLocation, Team* sourceTeam,Team* targetTeam);

void mouse(int button, int state, int x, int y);

#pragma endregion

#pragma region mapCreate

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
	if (pb != nullptr)
		pb->draw();
	if (pg != nullptr)
		pg->draw();

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
			PlaceItem(type, i);
			counter++;
		}
	}
}
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
#pragma endregion
//Iterate over the maze array and set the color according to it's value

// Restore Room Wall and Paths to their previous state
#pragma region CharcterNavgation
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


void RecoverTempGraysGhosts(int source, int target, Team* sourceTeam,Team* targetTeam, Character charcter)
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
			maze[charcter.getLocation()->getRow()][charcter.getLocation()->getCol()] = SPACE;
			charcter.getLocation()->setOldStatus(pCurrent->getOldStatus());
			charcter.getLocation()->setCol(col);
			charcter.getLocation()->setRow(row);
			maze[row][col] = source;
			continue;
		}
		else if (maze[row][col] == TARGET)
		{
			if (startGame == 1)
			{
				maze[row][col] = target;
				maze[charcter.getLocation()->getRow()][charcter.getLocation()->getCol()] = source;
			}
				
			continue;
		}
		maze[row][col] = pCurrent->getOldStatus();
	}
}

void RestorePathGhosts(Cell* pc,Team* sourceTeam) //add flag for running and change next row and nextCol logic to fit situation
{
	while (pc->getParent() != nullptr)
	{
		if (pc->getParent()->getParent() == nullptr)
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
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

//// Check distance
void CheckNeighborDistanceGhosts(Cell* pCurrent, int row, int col, int target, Team* team)
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
				//shot?
		}
		/*else
			cout << "Found Pacman Path" << endl;*/
		RestorePathGhosts(pCurrent, team);
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
bool FoundTarget(vector<Cell*> locations, int target)
{
	for (int i = 0; i < locations.size(); i++)
	{
		if (maze[locations[i]->getRow()][locations[i]->getCol()] != target)
			return false;
	}
	return true;
}

void AStarSearch(int target, Cell* currentLocation,Team* sourceTeam, Team* targetTeam)
{
	roomGrays.clear();
	drawPassages = true;
	Cell* pCurrent = currentLocation;
	roomGrays.push_back(pCurrent);


	tempGrays.push_back(currentLocation); // temp Grays will be reset to thier old value at the end


	while (FoundTarget(targetTeam->GetTargetByType(target), target))
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
			CheckNeighborDistanceGhosts(pCurrent, row + 1, col, target, sourceTeam);
		// DOWN
		if (drawPassages)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == target || (maze[row - 1][col] == AMMO) || (maze[row - 1][col] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row - 1, col, target, sourceTeam);
		// right		
		if (drawPassages)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == target || (maze[row][col + 1] == AMMO) || (maze[row][col + 1] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row, col + 1, target, sourceTeam);
		// left		
		if (drawPassages)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == target || (maze[row][col - 1] == AMMO) || (maze[row][col - 1] == HP))
				CheckNeighborDistanceGhosts(pCurrent, row, col - 1, target, sourceTeam);
	}
}
#pragma endregion



#pragma region gameLogic
void MoveTeams(int teamNum, int enemyTeam)
{
	teams[teamNum]->PlayTurn();
	//warrior 1
	int target = teams[teamNum]->warrior1->GetTarget(teamNum);
 	AStarSearch(target, teams[teamNum]->warrior1->getLocation(), teams[teamNum],teams[enemyTeam]);
 	RecoverTempGraysGhosts(WARRIOR_TEAM_1+ teamNum, target, teams[teamNum],teams[enemyTeam], *teams[teamNum]->warrior1);
	//warrior 2
	target = teams[teamNum]->warrior2->GetTarget(teamNum);
	AStarSearch(target, teams[teamNum]->warrior2->getLocation(), teams[teamNum],teams[enemyTeam]);
  	RecoverTempGraysGhosts(WARRIOR_TEAM_1 + teamNum, target, teams[teamNum],teams[enemyTeam], *teams[teamNum]->warrior2);
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
		MoveTeams(0, 1);
		MoveTeams(1, 0);
		sleep_for(milliseconds(50));
	}
	else
		return;

}
#pragma endregion

// runs all the time in the background
#pragma region main_exe
void idle()
{

	if (startGame == 1)
	{
		gameIteration();
	}

	if (fireBulet){
      fireBulet = pb->fire(maze);
	}

	if (throwGrenade) {
		pg->SimulateExplosion(maze, security_map);
	}
		
	glutPostRedisplay(); // indirect call to display
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//printf("clickEvent x %f, y %f, %f \n" , MSZ * x / (double)W, MSZ * (H - y) / (double)H, (rand() % 360) * PI / 180.0);
		pb = new Bullet(MSZ * x / (double)W, MSZ * (H - y) / (double)H, (rand() % 360) * PI / 180.0);
		pg = new Grenade(MSZ * x / (double)W, MSZ * (H - y) / (double)H);
	}
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // fire a bullet
		fireBulet = true;
		break;

	case 2: // throw grenade
		throwGrenade = true;
		break;


	case 3: // run Best First Search
		startGame = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(1200, 900);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(400, 200);
	glutCreateWindow("First Example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);

	glutCreateMenu(menu); // defines function menu as "menu" function
	
	glutAddMenuEntry("fire bullet", 1);
	glutAddMenuEntry("Throw grenade", 2);
     glutAddMenuEntry("Start Game", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // attach to right mouse button

	init();
	glutMainLoop();
}
#pragma endregion

