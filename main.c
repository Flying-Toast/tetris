#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "tetris.h"

#define ENABLE_DISCOTIME

#define SQUARE_SIZE (30)

#define VIEWPORT_PLAYFIELD_WIDTH (SQUARE_SIZE * PLAYFIELD_WIDTH)
#define VIEWPORT_QUEUE_WIDTH (250)
#define VIEWPORT_HOLD_WIDTH (250)

#define GHOST_OPACITY (100)

#define WINDOW_WIDTH (VIEWPORT_HOLD_WIDTH + VIEWPORT_PLAYFIELD_WIDTH + VIEWPORT_QUEUE_WIDTH)
#define WINDOW_HEIGHT (SQUARE_SIZE * VISIBLE_PLAYFIELD_HEIGHT)

#define PIECEBOX_WIDTH (SQUARE_SIZE * SHAPE_BOUNDING_BOX_SIZE)
#define PIECEBOX_HEIGHT (PIECEBOX_WIDTH)

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

static const Uint8 gameover_bg[] = {255, 10, 12};
static const Uint8 sidebar_bg[] = {55, 55, 55};
static const Uint8 piecebox_bg[] = {0, 0, 0};

static SDL_Window *win;
static SDL_Renderer *rndr;

static const SDL_Rect hold_rect = {
	.x = 0,
	.y = 0,
	.w = VIEWPORT_HOLD_WIDTH,
	.h = WINDOW_HEIGHT,
};

static const SDL_Rect hold_viewport = {
	.x = 0,
	.y = 0,
	.w = hold_rect.w,
	.h = hold_rect.h
};

static const SDL_Rect playfield_rect = {
	.x = 0,
	.y = 0,
	.w = VIEWPORT_PLAYFIELD_WIDTH,
	.h = WINDOW_HEIGHT
};

static const SDL_Rect playfield_viewport = {
	.x = VIEWPORT_HOLD_WIDTH,
	.y = 0,
	.w = playfield_rect.w,
	.h = playfield_rect.h
};

static const SDL_Rect queue_rect = {
	.x = 0,
	.y = 0,
	.w = VIEWPORT_PLAYFIELD_WIDTH,
	.h = WINDOW_HEIGHT
};

static const SDL_Rect queue_viewport = {
	.x = VIEWPORT_HOLD_WIDTH + VIEWPORT_PLAYFIELD_WIDTH,
	.y = 0,
	.w = queue_rect.w,
	.h = queue_rect.h
};

#ifdef ENABLE_DISCOTIME
static bool discotime = false;
#endif

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
	rect.x = x;
	rect.y = y;
	SDL_RenderFillRect(renderer, &rect);
}

static void render_playfield_square(enum square sq, int x, int y, bool ghost, SDL_Renderer *renderer)
{
	render_square(sq, x * SQUARE_SIZE, (y - PLAYFIELD_VISIBLE_START) * SQUARE_SIZE, ghost, renderer);
}

static void tetris_render_current_at_y(struct tetris *const t, int at_y, bool ghost, SDL_Renderer *renderer)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			enum square curr = t->current_tetromino.squares[y][x];
			render_playfield_square(curr, x + t->current_x, y + at_y, ghost, renderer);
		}
	}
}

static void tetris_render_current(struct tetris *const t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, t->current_y, false, renderer);
}

static void tetris_render_ghost(struct tetris *const t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, tetris_slammed_y(t), true, renderer);
}

static int tetromino_min_x(struct tetromino *const tm)
{
	for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return x - 1;
		}
	}

	return SHAPE_BOUNDING_BOX_SIZE - 2;
}

static int tetromino_min_y(struct tetromino *const tm)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return y - 1;
		}
	}

	return SHAPE_BOUNDING_BOX_SIZE - 2;
}

static int tetromino_max_x(struct tetromino *const tm)
{
	for (int x = SHAPE_BOUNDING_BOX_SIZE - 1; x > 0; x--) {
		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return x;
		}
	}

	return 0;
}

static int tetromino_max_y(struct tetromino *const tm)
{
	for (int y = SHAPE_BOUNDING_BOX_SIZE - 1; y > 0; y--) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return y;
		}
	}

	return 0;
}

static void tetris_render_hold(struct tetris *const t, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, sidebar_bg[0], sidebar_bg[1], sidebar_bg[2], 255);
	SDL_RenderFillRect(renderer, &hold_rect);

	SDL_Rect holdbox = {
		.x = (VIEWPORT_HOLD_WIDTH - PIECEBOX_WIDTH) / 2,
		.y = (WINDOW_HEIGHT - PIECEBOX_HEIGHT) / 4,
		.w = PIECEBOX_WIDTH,
		.h = PIECEBOX_HEIGHT
	};
	SDL_SetRenderDrawColor(renderer, piecebox_bg[0], piecebox_bg[1], piecebox_bg[2], 255);
	SDL_RenderFillRect(renderer, &holdbox);

	if (t->holding) {
		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
				int min_x = tetromino_min_x(&t->held_tetromino);
				int min_y = tetromino_min_y(&t->held_tetromino);
				int piece_width = (tetromino_max_x(&t->held_tetromino) - min_x) * SQUARE_SIZE;
				int piece_height = (tetromino_max_y(&t->held_tetromino) - min_y) * SQUARE_SIZE;
				int renderx = holdbox.x + ((PIECEBOX_WIDTH - piece_width) / 2) + (x * SQUARE_SIZE) - (((min_x + 1) * SQUARE_SIZE) / 2);
				int rendery = holdbox.y + ((PIECEBOX_HEIGHT - piece_height) / 2) + (y * SQUARE_SIZE) - (((min_y + 1) * SQUARE_SIZE) / 2);
				renderx -= ((min_x + 1) * SQUARE_SIZE) / 2;
				rendery -= ((min_y + 1) * SQUARE_SIZE) / 2;
				render_square(t->held_tetromino.squares[y][x], renderx, rendery, false, renderer);
			}
		}
	}
}

static void tetris_render_queue(struct tetris *const t, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, sidebar_bg[0], sidebar_bg[1], sidebar_bg[2], 255);
	SDL_RenderFillRect(renderer, &queue_rect);
}

static void tetris_render_playfield(struct tetris *const t, SDL_Renderer *renderer)
{
	if (t->gameover) {
		SDL_SetRenderDrawColor(renderer, gameover_bg[0], gameover_bg[1], gameover_bg[2], 255);
	} else {
		set_sdl_square_color(SQUARE_EMPTY, false, renderer);
	}
	SDL_RenderFillRect(renderer, &playfield_rect);

	for (int y = PLAYFIELD_VISIBLE_START; y < PLAYFIELD_HEIGHT; y++) {
		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			render_playfield_square(t->playfield[y][x], x, y, false, renderer);
		}
	}

	tetris_render_current(t, renderer);
	tetris_render_ghost(t, renderer);
}

static void tetris_render(struct tetris *const t, SDL_Renderer *renderer)
{
	SDL_RenderSetViewport(renderer, &hold_viewport);
	tetris_render_hold(t, renderer);

	SDL_RenderSetViewport(renderer, &playfield_viewport);
	tetris_render_playfield(t, renderer);

	SDL_RenderSetViewport(renderer, &queue_viewport);
	tetris_render_queue(t, renderer);
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
		tetris_render(&tet, rndr);
		SDL_RenderPresent(rndr);
	}
}
