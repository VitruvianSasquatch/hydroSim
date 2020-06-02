#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>

#include <stdbool.h>
#include <string.h>
#include <unistd.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_image.h>


#include <time.h>

#include "hydro.h"
#include "input/input.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int eventHandler(void *data, SDL_Event *event)
{
	bool *isRunning = (bool *)data;
	switch (event->type) {
		case SDL_QUIT:
			*isRunning = false; //This allows the current loop to finish. 
			break;
	}
	return 0; //eventHandler is not being used to filter events, so retVal is ignored. 
}



bool initSDL(const char *title, int width, int height, SDL_Window **window, SDL_Renderer **renderer)
{
	bool success = true;
	if ( SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
		success = false;
	}

	*window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN /*SDL_WINDOW_OPENGL*/);
	if (!*window) {
		fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
		success = false;
	} else {
		*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
		if (!*renderer) {
			fprintf(stderr, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			//Initialize renderer color
			SDL_SetRenderDrawColor(*renderer, 0xff, 0xff, 0xff, 0xff);

			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if ( !( IMG_Init(imgFlags) & imgFlags ) ) {
				fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
		}
	}
	return success;
}


void closeSDL(SDL_Window *window, SDL_Renderer *renderer)
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}





static const int keyAssignments[6] = {
	SDL_SCANCODE_A, 
	SDL_SCANCODE_D, 
	SDL_SCANCODE_SPACE, 
	SDL_SCANCODE_W, 
	SDL_SCANCODE_S, 
	SDL_SCANCODE_LEFTCLICK
};

#define RUN_IMPULSE 5000


void handleInput(void)
{
	if (input_justPressed(SDL_SCANCODE_LEFTCLICK)) {
		//dostuff();
	}
}


#define CELL_SIZE 50

void drawWorld(double head[WORLD_WIDTH][WORLD_HEIGHT], bool walls[WORLD_WIDTH][WORLD_HEIGHT], SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(renderer);
	
	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			SDL_Rect fillRect = {i*CELL_SIZE, j*CELL_SIZE, CELL_SIZE, CELL_SIZE};
			if (walls[i][j]) {
				SDL_SetRenderDrawColor(renderer, 210, 90, 0, 0xff);
			} else {
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, MIN(head[i][j], UINT8_MAX), 0xff);
			}
			SDL_RenderFillRect(renderer, &fillRect);
		}
	}

	SDL_RenderPresent(renderer);
}



int main(void)
{
	bool isRunning = true;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	initSDL("HydroSim", WINDOW_WIDTH, WINDOW_HEIGHT, &window, &renderer);

	SDL_AddEventWatch(&eventHandler, &isRunning); //Handles exit etc. 

	input_init();

	double head[WORLD_WIDTH][WORLD_HEIGHT] = {0};
	double v[WORLD_WIDTH][WORLD_HEIGHT] = {0};
	bool walls[WORLD_WIDTH][WORLD_HEIGHT] = {0};

	for (int i = 0; i < WORLD_HEIGHT; i++) {
		head[2][i] = 100;
	}


	while (isRunning) {

		input_update();
		handleInput();


		hydro_update(head, v, walls);
		drawWorld(head, walls, renderer);
		double sum = 0;
		for (int i = 0; i < WORLD_WIDTH; i++) {
			printf("%3.2f ", head[i][0]);
			sum += head[i][0];
		}
		printf(": %4.2f\n", sum);

		for (int i = 0; i < WORLD_WIDTH; i++) {
			printf("%3.2f ", v[i][0]);
		}
		printf("\n\n\n");

		//usleep(1e6);

	}

	closeSDL(window, renderer);

}
