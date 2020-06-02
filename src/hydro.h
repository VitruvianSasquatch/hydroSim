#ifndef HYDRO_H
#define HYDRO_H

#include <stdbool.h>


#define WORLD_WIDTH 15
#define WORLD_HEIGHT 10


typedef struct {
	double head;
	double vx;
	double vy;
	bool isWall;
} Cell_t;



void hydro_update(Cell_t world[WORLD_WIDTH][WORLD_HEIGHT]);





#endif //HYDRO_H