#ifndef HYDRO_H
#define HYDRO_H

#include <stdbool.h>


#define WORLD_WIDTH 5
#define WORLD_HEIGHT 1


void hydro_update(double head[WORLD_WIDTH][WORLD_HEIGHT], double v[WORLD_WIDTH][WORLD_HEIGHT], bool walls[WORLD_WIDTH][WORLD_HEIGHT]);





#endif //HYDRO_H