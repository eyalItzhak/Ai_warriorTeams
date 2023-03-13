#include "Bullet.h"
#include <math.h>
#include "glut.h"
#include <stdio.h>


Bullet::Bullet()
{
}

Bullet::Bullet(double xx, double yy, double alpha)
{
	x = xx;
	y = yy;
	angle = alpha;
	dirx = cos(alpha);
	diry = sin(alpha);
	hitTarget = false;
}

bool Bullet::fire(int maze[MSZ][MSZ] ,Team * target, double security_map[MSZ][MSZ])
{
	if (maze[(int)y][(int)x] == WALL) {
		hitTarget = true;
		return false;
	}
		

	if (maze[(int)y][(int)x] == target->getWarriorsType() && hitTarget==false){
		hitTarget = true;
		target->TeamMemeberTakeDamageEvent((int)x, (int)y, (DAMAGE)-(0.1* distance));
		return false;
	}	

	security_map[(int)y][(int)x] += MARK;
	x += dirx * STEP; // move bullet by STEP to direction (dirx,dity)
	y += diry * STEP;
	distance = distance + 1;

	return true;
}

void Bullet::draw()
{
	double delta = 0.5;
	glColor3d(0, 0, 0); // black
	glBegin(GL_POLYGON);
	glVertex2d(x - delta, y);
	glVertex2d(x, y + delta);
	glVertex2d(x + delta, y);
	glVertex2d(x, y - delta);
	
	glEnd();
	
}

void Bullet::SimulateFire(int maze[MSZ][MSZ], double security_map[MSZ][MSZ])
{
	while (maze[(int)y][(int)x] != WALL)
	{
		security_map[(int)y][(int)x] += MARK;
		x += dirx * STEP; // move bullet by STEP to direction (dirx,dity)
		y += diry * STEP;

	}
}

bool Bullet::isBulletHit()
{
	return hitTarget;
}


