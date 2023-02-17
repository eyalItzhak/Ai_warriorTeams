#pragma once
#include "State.h"
#include <vector>
class State;
class Cell
{
private:
	State* currentState;
	int row, col;
	float h, g, f;
	Cell* parent;
	int old_status;
	bool is_pacman;
	bool chasing;
	int timer;
public:
	Cell();
	Cell(int r, int c, Cell* p);
	~Cell();
	int getRow() { return row; }
	int getCol() { return col; }
	float getH() { return h; }
	float getG() { return g; }
	float getF() { return f; }
	bool getPacman() { return is_pacman; }
	bool getChasing() { return chasing; }
	int getOldStatus() { return old_status; }
	void setOldStatus(int val) { old_status = val; }
	void setH(float val) { h = val; }
	void setG(float val) { g = val; }
	void setRow(int r) { row = r; }
	void setCol(int c) { col = c; }
	void setF() { f = h + g; }
	void setPacman(bool val) { is_pacman = val; }
	void setChasing(bool val) { chasing = val; }
	void deleteParent() { parent = nullptr; }
	Cell* getParent() { return parent; }
	State* getCurrentState() { return currentState; }
	void setCurrentState(State* pNewState) { currentState = pNewState; }
	bool Execute();
};

