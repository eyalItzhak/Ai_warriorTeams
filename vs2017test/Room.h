#pragma once
#include "Cell.h"
class Room
{
private:
	int size, start_x, start_y;
	Cell* center;

public:
	Room();
	Room(Cell* c, int s_x, int s_y, int amount);
	~Room();
	Cell* getCenter() { return center; }
	int getSize() { return size; }
	int getStart_x() { return start_x; }
	int getStart_y() { return start_y; }

	Cell* getRoom();
};

