#include "Cell.h"
#include "State.h"
using namespace std;
#include <iostream>

static const int ENDTIME = 75;
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




