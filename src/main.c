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


#define CELL_SIZE 50


#define WINDOW_WIDTH (CELL_SIZE*WORLD_WIDTH)
#define WINDOW_HEIGHT (CELL_SIZE*WORLD_HEIGHT)

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



typedef enum {
	PLAY_PAUSE, 
	TOGGLE_WALL, 
	TOGGLE_WATER, 
	NUM_BINDINGS
} Binding_t;


static const Binding_t bindings[NUM_BINDINGS] = {
	SDL_SCANCODE_SPACE, 
	SDL_SCANCODE_LEFTCLICK,
	SDL_SCANCODE_RIGHTCLICK
};


void handleInput(Cell_t world[WORLD_WIDTH][WORLD_HEIGHT], bool *isSimRunning)
{
	if (input_justPressed(bindings[TOGGLE_WALL])) {
		int x = input_getMouseX();
		int y = input_getMouseY();

		if (world[x/CELL_SIZE][y/CELL_SIZE].isWall) {
			world[x/CELL_SIZE][y/CELL_SIZE].isWall = false;
		} else {
			world[x/CELL_SIZE][y/CELL_SIZE].head = 0;
			world[x/CELL_SIZE][y/CELL_SIZE].isWall = true;
		}
	}
	
	if (input_justPressed(bindings[TOGGLE_WATER])) {
		int x = input_getMouseX();
		int y = input_getMouseY();

		if (world[x/CELL_SIZE][y/CELL_SIZE].head > 50) {
			world[x/CELL_SIZE][y/CELL_SIZE].head = 0;
		} else {
			world[x/CELL_SIZE][y/CELL_SIZE].isWall = false;
			world[x/CELL_SIZE][y/CELL_SIZE].head = 100;
		}
	}

	if (input_justPressed(bindings[PLAY_PAUSE])) {
		*isSimRunning = !*isSimRunning;
	}
}


void drawWorld(Cell_t world[WORLD_WIDTH][WORLD_HEIGHT], SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(renderer);
	
	for (int i = 0; i < WORLD_WIDTH; i++) {
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			SDL_Rect fillRect = {i*CELL_SIZE, j*CELL_SIZE, CELL_SIZE, CELL_SIZE};
			if (world[i][j].isWall) {
				SDL_SetRenderDrawColor(renderer, 210, 90, 0, 0xff);
			} else {
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, MIN(4*world[i][j].head, UINT8_MAX), 0xff);
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

	Cell_t world[WORLD_WIDTH][WORLD_HEIGHT] = {0};



	bool isSimRunning = false;

	while (isRunning) {

		input_update();
		handleInput(world, &isSimRunning);

		if (isSimRunning) {
			hydro_update(world);
			double sum = 0;
			for (int i = 0; i < WORLD_WIDTH; i++) {
				for (int j = 0; j < WORLD_HEIGHT; j++) {
					sum += world[i][j].head;
				}
			}
			printf("%4.2f\n", sum);
		}

		drawWorld(world, renderer);
		


		//usleep(1e6);

	}

	closeSDL(window, renderer);

}
