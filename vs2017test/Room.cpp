#include "Room.h"


Room::Room()
{
}

Room::Room(Cell* c,int s_x, int s_y, int amount)
{
	center = c;
	start_x = s_x;
	start_y = s_y;
	size = amount;
}


Room::~Room()
{
}