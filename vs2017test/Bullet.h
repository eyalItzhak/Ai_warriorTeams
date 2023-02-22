#pragma once
#include "Enum.h"

const double STEP = 0.2;
const double MARK = 0.001;

class Bullet
{
private:
	double x, y;
	double dirx, diry;
	double angle;
public:
	Bullet();
	Bullet(double xx, double yy, double alpha);
	bool fire(int maze[MSZ][MSZ]);
	void draw();
	void SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ]);
};

