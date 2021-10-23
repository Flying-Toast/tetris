#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "tetris.h"

#define SQUARE_SIZE (30)
#define WINDOW_WIDTH (SQUARE_SIZE * PLAYFIELD_WIDTH)
#define WINDOW_HEIGHT (SQUARE_SIZE * VISIBLE_PLAYFIELD_HEIGHT)
#define ENABLE_DISCOTIME
#define GHOST_OPACITY (100)

static SDL_Window *win;
static SDL_Renderer *rndr;

#ifdef ENABLE_DISCOTIME
static bool discotime = false;
#endif

static const Uint8 squarecolors[][3] = {
	[SQUARE_EMPTY] = {0, 0, 0},
	[SQUARE_CYAN] = {2, 248, 252},
	[SQUARE_BLUE] = {34, 51, 232},
	[SQUARE_ORANGE] = {239, 160, 14},
	[SQUARE_YELLOW] = {247, 235, 12},
	[SQUARE_GREEN] = {43, 163, 32},
	[SQUARE_RED] = {226, 50, 34},
	[SQUARE_PURPLE] = {120, 11, 183}
};

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

static void set_sdl_square_color(enum square sq, bool ghost, SDL_Renderer *renderer)
{
	if (ghost) {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], GHOST_OPACITY);
	} else {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], 255);
	}
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

static void render_square(enum square sq, int x, int y, bool ghost, SDL_Renderer *renderer)
{
	if (sq == SQUARE_EMPTY)
		return;

#ifdef ENABLE_DISCOTIME
	if (discotime) {
		if (ghost) {
			SDL_SetRenderDrawColor(renderer, rand(), rand(), rand(), GHOST_OPACITY);
		} else {
			SDL_SetRenderDrawColor(renderer, rand(), rand(), rand(), 255);
		}
	} else {
#endif
		set_sdl_square_color(sq, ghost, renderer);
#ifdef ENABLE_DISCOTIME
	}
#endif
	SDL_Rect rect;
	rect.w = SQUARE_SIZE;
	rect.h = SQUARE_SIZE;
	rect.x = x * SQUARE_SIZE;
	rect.y = (y - PLAYFIELD_VISIBLE_START) * SQUARE_SIZE;
	SDL_RenderFillRect(renderer, &rect);
}

static void tetris_render_current_at_y(struct tetris *const t, int at_y, bool ghost, SDL_Renderer *renderer)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			enum square curr = t->current_tetromino.squares[y][x];
			render_square(curr, x + t->current_x, y + at_y, ghost, renderer);
		}
	}
}

static void tetris_render_current(struct tetris *const t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, t->current_y, false, renderer);
}

static void tetris_render_ghost(struct tetris *const t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, t->slam_y, true, renderer);
}

static void tetris_render(struct tetris *const t, SDL_Renderer *renderer)
{
	set_sdl_square_color(SQUARE_EMPTY, false, renderer);
	SDL_RenderClear(renderer);

	for (int y = PLAYFIELD_VISIBLE_START; y < PLAYFIELD_HEIGHT; y++) {
		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			render_square(t->playfield[y][x], x, y, false, renderer);
		}
	}

	tetris_render_current(t, renderer);
	tetris_render_ghost(t, renderer);
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
					tetris_move_current_left(&tet);
				if (e.key.keysym.scancode == SDL_SCANCODE_RIGHT)
					tetris_move_current_right(&tet);
				if (e.key.keysym.scancode == SDL_SCANCODE_DOWN)
					tet.tick_interval = FAST_TICK_INTERVAL;
				if (e.key.keysym.scancode == SDL_SCANCODE_SPACE)
					tetris_slam(&tet);
			} else if (e.type == SDL_KEYUP) {
				if (!kstate[SDL_SCANCODE_DOWN])
					tet.tick_interval = DEFAULT_TICK_INTERVAL;
			}
		}

		if (now_tick - last_tick >= tet.tick_interval) {
			tetris_tick(&tet);
			last_tick = now_tick;
		}
		tetris_render(&tet, rndr);
		SDL_RenderPresent(rndr);
	}
}
