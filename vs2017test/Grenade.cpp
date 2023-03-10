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
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (bullets[i] != nullptr) {

		Bullet *currentBullet = bullets[i];
		if (currentBullet->isBulletHit() == false) {
			currentBullet->draw();
		}
	
		}
		
	}
		

}

bool Grenade::explode(int maze[MSZ][MSZ] , Team* target)
{
	bool finished = true;
	for (int i = 0; i < NUM_BULLETS; i++)
		if (bullets[i]->fire(maze, target))
			finished = false;

	return !finished;
}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double security_map[MSZ][MSZ],Team * target)
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->fire(maze, target);  //wall it target temp...

	//was bullets[i]->SimulateFire(maze, security_map);

}

bool Grenade::isExplodeStop()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		if (bullets[i]->isBulletHit() == false) 
		{
			return false;
		}
		
	   return true;
		
}
