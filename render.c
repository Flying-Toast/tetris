#include <SDL2/SDL.h>
#include "render.h"

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
bool discotime = false;
#endif

static void set_sdl_square_color(enum square sq, bool ghost, SDL_Renderer *renderer)
{
	if (ghost) {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], GHOST_OPACITY);
	} else {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], 255);
	}
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

static void tetris_render_current_at_y(const struct tetris *t, int at_y, bool ghost, SDL_Renderer *renderer)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			enum square curr = t->current_tetromino.squares[y][x];
			render_playfield_square(curr, x + t->current_x, y + at_y, ghost, renderer);
		}
	}
}

static void tetris_render_current(const struct tetris *t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, t->current_y, false, renderer);
}

static void tetris_render_ghost(const struct tetris *t, SDL_Renderer *renderer)
{
	tetris_render_current_at_y(t, tetris_slammed_y(t), true, renderer);
}

static int tetromino_min_x(const struct tetromino *tm)
{
	for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return x - 1;
		}
	}

	return SHAPE_BOUNDING_BOX_SIZE - 2;
}

static int tetromino_min_y(const struct tetromino *tm)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return y - 1;
		}
	}

	return SHAPE_BOUNDING_BOX_SIZE - 2;
}

static int tetromino_max_x(const struct tetromino *tm)
{
	for (int x = SHAPE_BOUNDING_BOX_SIZE - 1; x > 0; x--) {
		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return x;
		}
	}

	return 0;
}

static int tetromino_max_y(const struct tetromino *tm)
{
	for (int y = SHAPE_BOUNDING_BOX_SIZE - 1; y > 0; y--) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			if (tm->squares[y][x] != SQUARE_EMPTY)
				return y;
		}
	}

	return 0;
}

static void tetris_render_hold(const struct tetris *t, SDL_Renderer *renderer)
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

static void tetris_render_queue(const struct tetris *t, SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, sidebar_bg[0], sidebar_bg[1], sidebar_bg[2], 255);
	SDL_RenderFillRect(renderer, &queue_rect);

	for (int i = 0; i < QUEUE_LENGTH; i++) {
		SDL_Rect box = {
			.x = (VIEWPORT_HOLD_WIDTH - PIECEBOX_WIDTH) / 2,
			.y = ((WINDOW_HEIGHT - PIECEBOX_HEIGHT) / 4) + (PIECEBOX_HEIGHT * i),
			.w = PIECEBOX_WIDTH,
			.h = PIECEBOX_HEIGHT
		};
		SDL_SetRenderDrawColor(renderer, piecebox_bg[0], piecebox_bg[1], piecebox_bg[2], 255);
		SDL_RenderFillRect(renderer, &box);

		for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
			for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
				const struct tetromino *curr = &t->queue[tetris_queue_real_index(t, i)];
				int min_x = tetromino_min_x(curr);
				int min_y = tetromino_min_y(curr);
				int piece_width = (tetromino_max_x(curr) - min_x) * SQUARE_SIZE;
				int piece_height = (tetromino_max_y(curr) - min_y) * SQUARE_SIZE;
				int renderx = box.x + ((PIECEBOX_WIDTH - piece_width) / 2) + (x * SQUARE_SIZE) - (((min_x + 1) * SQUARE_SIZE) / 2);
				int rendery = box.y + ((PIECEBOX_HEIGHT - piece_height) / 2) + (y * SQUARE_SIZE) - (((min_y + 1) * SQUARE_SIZE) / 2);
				renderx -= ((min_x + 1) * SQUARE_SIZE) / 2;
				rendery -= ((min_y + 1) * SQUARE_SIZE) / 2;
				render_square(curr->squares[y][x], renderx, rendery, false, renderer);
			}
		}
	}
}

static void tetris_render_playfield(const struct tetris *t, SDL_Renderer *renderer)
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

void tetris_render(const struct tetris *t, SDL_Renderer *renderer)
{
	SDL_RenderSetViewport(renderer, &hold_viewport);
	tetris_render_hold(t, renderer);

	SDL_RenderSetViewport(renderer, &playfield_viewport);
	tetris_render_playfield(t, renderer);

	SDL_RenderSetViewport(renderer, &queue_viewport);
	tetris_render_queue(t, renderer);
}
