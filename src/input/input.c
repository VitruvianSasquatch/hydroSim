#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>

#include "input.h"


static const uint8_t *keyboardState = NULL;
static uint8_t *prevKeyboardState = NULL;
static int numKeys = 0;
static uint32_t mouseState = 0;
static uint32_t prevMouseState = 0;
static int mouseX = 0;
static int mouseY = 0;


void input_init(void)
{
	SDL_PumpEvents();
	keyboardState = SDL_GetKeyboardState(&numKeys);
	mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	prevKeyboardState = malloc(sizeof(uint8_t)*numKeys);
}

//TODO: Free prevKeyboardState? Add an exit function?



void input_update(void)
{
	memcpy(prevKeyboardState, keyboardState, numKeys);
	prevMouseState = mouseState;

	SDL_PumpEvents(); //autoupdate keyboardState. 
	mouseState = SDL_GetMouseState(&mouseX, &mouseY);
}


int input_getMouseX(void)
{
	return mouseX;
}


int input_getMouseY(void)
{
	return mouseY;
}


int input_isDown(int code)
{
	if (code < 0) {
		return mouseState & SDL_BUTTON(-code); //Negatives ensure exclusivity with keyboard
	} else {
		return keyboardState[code];
	}
}


int input_justPressed(int code)
{
	if (code < 0) {
		return !(prevMouseState & SDL_BUTTON(-code)) && (mouseState & SDL_BUTTON(-code)); //Negatives ensure exclusivity with keyboard
	} else {
		return !prevKeyboardState[code] && keyboardState[code];
	}
}


int input_justReleased(int code)
{
	if (code < 0) {
		return (prevMouseState & SDL_BUTTON(-code)) && !(mouseState & SDL_BUTTON(-code)); //Negatives ensure exclusivity with keyboard
	} else {
		return prevKeyboardState[code] && !keyboardState[code];
	}
}