#pragma once
#include "Enum.h"
#include "Team.h"

const double STEP = 0.2;
const double MARK = 0.001;


class Bullet
{
private:
    int distance = 0;
	double x, y;
	double dirx, diry;
	double angle;
	bool hitTarget;

public:

	Bullet();
	Bullet(double xx, double yy, double alpha);
	bool fire(int maze[MSZ][MSZ], Team* target, double security_map[MSZ][MSZ]);
	void draw();
	void SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ]);
	bool isBulletHit();
};

