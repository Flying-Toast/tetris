#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "tetris.h"
#include "render.h"

static SDL_Window *win;
static SDL_Renderer *rndr;

static void seed_rng(void)
{
	srand(time(NULL));
}

static void clean_exit(int err)
{
	SDL_DestroyRenderer(rndr);
	SDL_DestroyWindow(win);
	SDL_Quit();
	exit(err);
}

static void die_with_sdl_err(void)
{
	printf("SDL error: %s\nExiting...\n", SDL_GetError());
	clean_exit(1);
}

static void init(void)
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
	SDL_SetRenderDrawBlendMode(rndr, SDL_BLENDMODE_BLEND);
}

int main(int argc, char **argv)
{
	init();
	struct tetris tet;
	tetris_init(&tet);
#ifdef ENABLE_DISCOTIME
	if (strstr(argv[0], "discotime"))
		discotime = true;
#endif

	Uint32 last_tick = SDL_GetTicks();
	const Uint8 *kstate = SDL_GetKeyboardState(NULL);
	for (;;) {
		Uint32 now_tick = SDL_GetTicks();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				clean_exit(0);
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.scancode == SDL_SCANCODE_LEFT)
					tetris_move_current(&tet, XDIRECTION_LEFT);
				if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT)
					tetris_move_current(&tet, XDIRECTION_RIGHT);
				if (e.key.keysym.scancode == SDL_SCANCODE_DOWN)
					tet.tick_interval = FAST_TICK_INTERVAL;
				if (e.key.keysym.scancode == SDL_SCANCODE_SPACE)
					tetris_slam(&tet);
				if (e.key.keysym.scancode == SDL_SCANCODE_X)
					tetris_rotate_current(&tet, ROTATION_CW);
				if (e.key.keysym.scancode == SDL_SCANCODE_Z)
					tetris_rotate_current(&tet, ROTATION_CCW);
				if (e.key.keysym.scancode == SDL_SCANCODE_C)
					tetris_hold(&tet);
			} else if (e.type == SDL_KEYUP) {
				if (!kstate[SDL_SCANCODE_DOWN])
					tet.tick_interval = DEFAULT_TICK_INTERVAL;
			} else if (e.type == SDL_MOUSEBUTTONDOWN && tet.gameover) {
				tetris_init(&tet);
			}
		}

		if (now_tick - last_tick >= tet.tick_interval) {
			tetris_tick(&tet);
			last_tick = now_tick;
		}
		tetris_render(rndr, &tet);
		SDL_RenderPresent(rndr);
	}
}
