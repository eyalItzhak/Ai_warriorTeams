#include "Grenade.h"

Grenade::Grenade(double xx, double yy)
{
	double alpha, teta = (360.0 / NUM_BULLETS) * PI / 180;
	int i;
	x = xx;
	y = yy;
	for (i = 0, alpha = 0; i < NUM_BULLETS; i++, alpha += teta)
		bullets[i] = new Bullet(xx, yy, alpha);
}

void Grenade::draw()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->draw();

}

bool Grenade::explode(int maze[MSZ][MSZ])
{
	bool finished = true;
	for (int i = 0; i < NUM_BULLETS; i++)
		if (bullets[i]->fire(maze))
			finished = false;

	return !finished;
}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double security_map[MSZ][MSZ])
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->fire(maze);

	//was bullets[i]->SimulateFire(maze, security_map);

}
