#include <stdio.h>
#include <sys/random.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

SDL_Window *win;
SDL_Surface *surf;

void seed_rng(void)
{
	unsigned int seed;
	getrandom(&seed, sizeof(seed), 0);
	srand(seed);
}

void die_with_sdl_err(void)
{
	printf("SDL error: %s\nExiting...\n", SDL_GetError());
	SDL_DestroyWindow(win);
	SDL_Quit();
	exit(1);
}

int main(int argc, char **argv)
{
	seed_rng();

	if (SDL_Init(SDL_INIT_VIDEO))
		die_with_sdl_err();

	win = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, SDL_WINDOW_SHOWN);
	if (!win)
		die_with_sdl_err();

	surf = SDL_GetWindowSurface(win);

	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 255, 0));
	SDL_UpdateWindowSurface(win);
	SDL_Delay(3000);
}
