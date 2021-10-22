#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <SDL2/SDL.h>
#include "tetris.h"

#define SQUARE_SIZE (30)
#define WINDOW_WIDTH (SQUARE_SIZE * PLAYFIELD_WIDTH)
#define WINDOW_HEIGHT (SQUARE_SIZE * PLAYFIELD_VISIBLE_HEIGHT)

SDL_Window *win;
SDL_Renderer *rndr;

const Uint8 squarecolors[][3] = {
	// 0 - SQUARE_EMPTY
	{0, 0, 0},
	// 1 - SQUARE_CYAN
	{2, 248, 252},
	// 2 - SQUARE_BLUE
	{34, 51, 232},
	// 3 - SQUARE_ORANGE
	{239, 160, 14},
	// 4 - SQUARE_YELLOW
	{247, 235, 12},
	// 5 - SQUARE_GREEN
	{43, 163, 32},
	// 6 - SQUARE_RED
	{226, 50, 34},
	// 7 - SQUARE_PURPLE
	{120, 11, 183}
};

void seed_rng(void)
{
	unsigned int seed;
	getrandom(&seed, sizeof(seed), 0);
	srand(seed);
}

void clean_exit(int err)
{
	SDL_DestroyRenderer(rndr);
	SDL_DestroyWindow(win);
	SDL_Quit();
	exit(err);
}

void die_with_sdl_err(void)
{
	printf("SDL error: %s\nExiting...\n", SDL_GetError());
	clean_exit(1);
}

void set_sdl_square_color(enum square sq)
{
	SDL_SetRenderDrawColor(rndr, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], 255);
}

void init(void)
{
	seed_rng();

	if (SDL_Init(SDL_INIT_VIDEO))
		die_with_sdl_err();

	win = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!win)
		die_with_sdl_err();

	rndr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (!rndr)
		die_with_sdl_err();
}

void render_square(enum square sq, int x, int y, SDL_Renderer *renderer)
{
	set_sdl_square_color(sq);
	SDL_Rect rect;
	rect.w = SQUARE_SIZE;
	rect.h = SQUARE_SIZE;
	rect.x = x * SQUARE_SIZE;
	rect.y = y * SQUARE_SIZE;
	SDL_RenderFillRect(renderer, &rect);
}

void tetris_render(struct tetris *const t, SDL_Renderer *renderer)
{
	for (int y = 0; y < PLAYFIELD_VISIBLE_HEIGHT; y++) {
		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			render_square(t->playfield[y][x], x, y, renderer);
		}
	}
}

int main(int argc, char **argv)
{
	init();
	struct tetris tet;
	tetris_init(&tet);

	Uint32 last_tick = SDL_GetTicks();
	for (;;) {
		Uint32 now_tick = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				clean_exit(0);
		}

		if (now_tick - last_tick >= tet.tick_interval) {
			tetris_tick(&tet);
			last_tick = now_tick;
		}
		tetris_render(&tet, rndr);
		SDL_RenderPresent(rndr);
	}
}
