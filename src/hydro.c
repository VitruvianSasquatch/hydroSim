#include <time.h>
#include <unistd.h>
#include <stdbool.h>


#include "hydro.h"

#include <stdio.h>




static inline double doubleClock(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return (double)t.tv_sec + (double)t.tv_nsec/1e9;
}






int getNeighbours(double *neighbours[], double head[WORLD_WIDTH][WORLD_HEIGHT], bool walls[WORLD_WIDTH][WORLD_HEIGHT], int x, int y)
{
	int i = 0;
	if (x > 0 && !walls[x-1][y]) {
		neighbours[i++] = &head[x-1][y];
	}
	if (x < WORLD_WIDTH-1 && !walls[x+1][y]) {
		neighbours[i++] = &head[x+1][y];
	}
	if (y > 0 && !walls[x][y-1]) {
		neighbours[i++] = &head[x][y-1];
	}
	if (y < WORLD_HEIGHT-1 && !walls[x][y+1]) {
		neighbours[i++] = &head[x][y+1];
	}
	return i;
}


void hydro_update(double head[WORLD_WIDTH][WORLD_HEIGHT], double v[WORLD_WIDTH][WORLD_HEIGHT], bool walls[WORLD_WIDTH][WORLD_HEIGHT])
{
	static double tPrev = -1;
	if (tPrev == -1) {
		tPrev = doubleClock();
		return;
	}

	double tNow = doubleClock();
	double dt = tNow - tPrev;
	//dt=1;
	tPrev = tNow;

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			double avg = 0;
			
			double *neighbours[4] = {NULL};
			int n = getNeighbours(neighbours, head, walls, i, j);
			for (int k = 0; k < n; k++) {
				avg += *neighbours[k];
			}
			avg /= n;

			//Assume density is 1, column area is the same, 
			v[i][j] += (avg-head[i][j])*dt;
		}
	}

	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			head[i][j] += v[i][j]*dt;
		}
	}
}