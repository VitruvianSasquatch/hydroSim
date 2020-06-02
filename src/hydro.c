#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "hydro.h"

#include <stdio.h>


#define SIGN(x) ( (x) == 0 ? 0 : ( (x) > 0 ? 1 : -1 ) ) /*Returns 0 if sign is 0; supports floating */



static inline double doubleClock(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec/1e9;
}



static inline bool isFillable(Cell_t world[WORLD_WIDTH][WORLD_HEIGHT], int x, int y)
{
	return (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT && !world[x][y].isWall);
}


int getNeighbours(int neighbours[4][2], Cell_t world[WORLD_WIDTH][WORLD_HEIGHT], int x, int y)
{
	int i = 0;
	if (x > 0 && !world[x-1][y].isWall) {
		neighbours[i][0] = x-1;
		neighbours[i][1] = y;
		i++;
	}
	if (x < WORLD_WIDTH-1 && !world[x+1][y].isWall) {
		neighbours[i][0] = x+1;
		neighbours[i][1] = y;
		i++;
	}
	if (y > 0 && !world[x][y-1].isWall) {
		neighbours[i][0] = x;
		neighbours[i][1] = y-1;
		i++;
	}
	if (y < WORLD_HEIGHT-1 && !world[x][y+1].isWall) {
		neighbours[i][0] = x;
		neighbours[i][1] = y+1;
		i++;
	}
	return i;
}


int getNeighbourDirs(int neighbours[4][2], Cell_t world[WORLD_WIDTH][WORLD_HEIGHT], int x, int y)
{
	int i = 0;
	if (x > 0 && !world[x-1][y].isWall) {
		neighbours[i][0] = -1;
		neighbours[i][1] = 0;
		i++;
	}
	if (x < WORLD_WIDTH-1 && !world[x+1][y].isWall) {
		neighbours[i][0] = 1;
		neighbours[i][1] = 0;
		i++;
	}
	if (y > 0 && !world[x][y-1].isWall) {
		neighbours[i][0] = 0;
		neighbours[i][1] = -1;
		i++;
	}
	if (y < WORLD_HEIGHT-1 && !world[x][y+1].isWall) {
		neighbours[i][0] = 0;
		neighbours[i][1] = 1;
		i++;
	}
	return i;
}


void hydro_update(Cell_t world[WORLD_WIDTH][WORLD_HEIGHT])
{
	static double tPrev = -1;
	if (tPrev == -1) {
		tPrev = doubleClock();
		return;
	}

	double tNow = doubleClock();
	double dt = tNow - tPrev;
	dt*=5;
	tPrev = tNow;

	static Cell_t diff[WORLD_WIDTH][WORLD_HEIGHT];
	memset(diff, 0, WORLD_WIDTH*WORLD_HEIGHT*sizeof(Cell_t));

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			
			int neighbourDirs[4][2];
			int n = getNeighbourDirs(neighbourDirs, world, i, j);

			if (world[i][j].head > 5 && n > 0) { //If the cell has water to give (avoids div by 0) and has neighbours to interact with. 

				double vSquared = world[i][j].vx*world[i][j].vx + world[i][j].vy*world[i][j].vy;
				double headFraction = fabs(world[i][j].head)/(fabs(world[i][j].head) + sqrt(vSquared)); //TODO: This is pulled out of an orafice. 
				double amountLost = world[i][j].head;

				double headAmountEach = headFraction*amountLost/n;

				for (int k = 0; k < n; k++) {
					int x = i + neighbourDirs[k][0];
					int y = j + neighbourDirs[k][1];
					diff[i][j].head -= headAmountEach;
					diff[x][y].head += headAmountEach;
					diff[x][y].vx += neighbourDirs[k][0]*headAmountEach; //TODO: might need scaling. 
					diff[x][y].vy += neighbourDirs[k][1]*headAmountEach;
				}


				if (vSquared > 0) { //Has some velocity

					double vAmount = amountLost*(1-headFraction); //This remaining amount will be moved by velocity. 
					double vxFraction = fabs(world[i][j].vx) / (fabs(world[i][j].vx) + fabs(world[i][j].vy));

					int dx = SIGN(world[i][j].vx);
					int dy = SIGN(world[i][j].vy);
					if (isFillable(world, i+dx, j)) {
						//Move the water:
						diff[i][j].head -= vxFraction*vAmount; 
						diff[i+dx][j].head += vxFraction*vAmount; 

						//Move the energy:
						diff[i][j].vx -= world[i][j].vx;
						diff[i+dx][j].vx += world[i][j].vx;
					} else {
						diff[i][j].vx -= 2*world[i][j].vx; //reflect
					}

					if (isFillable(world, i, j+dy)) {
						//Move the water
						diff[i][j].head -= (1-vxFraction)*vAmount;
						diff[i][j+dy].head += (1-vxFraction)*vAmount; 

						//Move the energy:
						diff[i][j].vy -= world[i][j].vy;
						diff[i][j+dy].vy += world[i][j].vy;
					} else {
						diff[i][j].vy -= 2*world[i][j].vy; //reflect
					}
				}

			}
		}
	}


	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			world[i][j].head += diff[i][j].head * dt;
			world[i][j].vx += diff[i][j].vx * dt;
			world[i][j].vy += diff[i][j].vy * dt;
		}
	}

}