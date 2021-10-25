#include <SDL2/SDL.h>
#include "render.h"

#define R_BYTE(rgb) (rgb >> 16)
#define G_BYTE(rgb) (rgb >> 8 & 0xFF)
#define B_BYTE(rgb) (rgb & 0xFF)

#define COLOR_BYTES_ARR(rgb) {R_BYTE(rgb), G_BYTE(rgb), B_BYTE(rgb)}

static const Uint8 squarecolors[][3] = {
	[SQUARE_EMPTY] = COLOR_BYTES_ARR(EMPTY_SQUARE_COLOR),
	[SQUARE_CYAN] = COLOR_BYTES_ARR(CYAN),
	[SQUARE_BLUE] = COLOR_BYTES_ARR(BLUE),
	[SQUARE_ORANGE] = COLOR_BYTES_ARR(ORANGE),
	[SQUARE_YELLOW] = COLOR_BYTES_ARR(YELLOW),
	[SQUARE_GREEN] = COLOR_BYTES_ARR(GREEN),
	[SQUARE_RED] = COLOR_BYTES_ARR(RED),
	[SQUARE_PURPLE] = COLOR_BYTES_ARR(PURPLE)
};

static const Uint8 gameover_bg[] = COLOR_BYTES_ARR(GAMEOVER_COLOR);
static const Uint8 sidebar_bg[] = COLOR_BYTES_ARR(SIDEBARS_BG_COLOR);
static const Uint8 piecebox_bg[] = COLOR_BYTES_ARR(PIECEBOX_BG_COLOR);

static const SDL_Rect hold_viewport = {
	.x = 0,
	.y = 0,
	.w = VIEWPORT_HOLD_WIDTH,
	.h = WINDOW_HEIGHT
};

static const SDL_Rect playfield_viewport = {
	.x = VIEWPORT_HOLD_WIDTH,
	.y = 0,
	.w = VIEWPORT_PLAYFIELD_WIDTH,
	.h = WINDOW_HEIGHT
};

static const SDL_Rect queue_viewport = {
	.x = VIEWPORT_HOLD_WIDTH + VIEWPORT_PLAYFIELD_WIDTH,
	.y = 0,
	.w = VIEWPORT_QUEUE_WIDTH,
	.h = WINDOW_HEIGHT
};

#ifdef ENABLE_DISCOTIME
bool discotime = false;
#endif

static void clear_current_viewport(SDL_Renderer *renderer, const SDL_Rect *viewport)
{
	SDL_Rect area = {
		.x = 0,
		.y = 0,
		.w = viewport->w,
		.h = viewport->h
	};
	SDL_RenderFillRect(renderer, &area);
}

static void set_sdl_square_color(SDL_Renderer *renderer, enum square sq, bool ghost)
{
	if (ghost) {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], GHOST_OPACITY);
	} else {
		SDL_SetRenderDrawColor(renderer, squarecolors[sq][0], squarecolors[sq][1], squarecolors[sq][2], 255);
	}
}

static void render_square(SDL_Renderer *renderer, enum square sq, int x, int y, bool ghost)
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
		set_sdl_square_color(renderer, sq, ghost);
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

static void render_playfield_square(SDL_Renderer *renderer, enum square sq, int x, int y, bool ghost)
{
	render_square(renderer, sq, x * SQUARE_SIZE, (y - PLAYFIELD_VISIBLE_START) * SQUARE_SIZE, ghost);
}

static void render_current_at_y(SDL_Renderer *renderer, const struct tetris *t, int at_y, bool ghost)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			enum square curr = t->current_tetromino.squares[y][x];
			render_playfield_square(renderer, curr, x + t->current_x, y + at_y, ghost);
		}
	}
}

static void render_current(SDL_Renderer *renderer, const struct tetris *t)
{
	render_current_at_y(renderer, t, t->current_y, false);
}

static void render_ghost(SDL_Renderer *renderer, const struct tetris *t)
{
	render_current_at_y(renderer, t, tetris_slammed_y(t), true);
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

static void render_empty_piecebox(SDL_Renderer *renderer, int x, int y)
{
	SDL_SetRenderDrawColor(renderer, piecebox_bg[0], piecebox_bg[1], piecebox_bg[2], 255);
	SDL_Rect box = {
		.x = x,
		.y = y,
		.w = SHAPE_BOUNDS_PX,
		.h = SHAPE_BOUNDS_PX
	};
	SDL_RenderFillRect(renderer, &box);
}

static void render_piece_in_box(SDL_Renderer *renderer, const struct tetromino *piece, int box_x, int box_y)
{
	render_empty_piecebox(renderer, box_x, box_y);

	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			int min_x = tetromino_min_x(piece);
			int min_y = tetromino_min_y(piece);
			int piece_width = (tetromino_max_x(piece) - min_x) * SQUARE_SIZE;
			int piece_height = (tetromino_max_y(piece) - min_y) * SQUARE_SIZE;
			int renderx = box_x + ((SHAPE_BOUNDS_PX - piece_width) / 2) + (x * SQUARE_SIZE) - (((min_x + 1) * SQUARE_SIZE) / 2);
			int rendery = box_y + ((SHAPE_BOUNDS_PX - piece_height) / 2) + (y * SQUARE_SIZE) - (((min_y + 1) * SQUARE_SIZE) / 2);
			renderx -= ((min_x + 1) * SQUARE_SIZE) / 2;
			rendery -= ((min_y + 1) * SQUARE_SIZE) / 2;
			render_square(renderer, piece->squares[y][x], renderx, rendery, false);
		}
	}
}

static void render_hold(SDL_Renderer *renderer, const struct tetris *t)
{
	SDL_SetRenderDrawColor(renderer, sidebar_bg[0], sidebar_bg[1], sidebar_bg[2], 255);
	clear_current_viewport(renderer, &hold_viewport);

	int box_x = (VIEWPORT_HOLD_WIDTH - SHAPE_BOUNDS_PX) / 2;
	int box_y = (WINDOW_HEIGHT - SHAPE_BOUNDS_PX) / 4;

	if (t->holding) {
		render_piece_in_box(renderer, &t->held_tetromino, box_x, box_y);
	} else {
		render_empty_piecebox(renderer, box_x, box_y);
	}
}

static void render_queue(SDL_Renderer *renderer, const struct tetris *t)
{
	SDL_SetRenderDrawColor(renderer, sidebar_bg[0], sidebar_bg[1], sidebar_bg[2], 255);
	clear_current_viewport(renderer, &queue_viewport);

	for (int i = 0; i < QUEUE_LENGTH; i++) {
		const struct tetromino *curr = tetris_queue_get(t, i);
		int box_x = (VIEWPORT_HOLD_WIDTH - SHAPE_BOUNDS_PX) / 2;
		int box_y = ((WINDOW_HEIGHT - SHAPE_BOUNDS_PX) / 4) + (SHAPE_BOUNDS_PX * i);
		render_piece_in_box(renderer, curr, box_x, box_y);
	}
}

static void render_playfield(SDL_Renderer *renderer, const struct tetris *t)
{
	if (t->gameover) {
		SDL_SetRenderDrawColor(renderer, gameover_bg[0], gameover_bg[1], gameover_bg[2], 255);
	} else {
		set_sdl_square_color(renderer, SQUARE_EMPTY, false);
	}
	clear_current_viewport(renderer, &playfield_viewport);

	for (int y = PLAYFIELD_VISIBLE_START; y < PLAYFIELD_HEIGHT; y++) {
		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			render_playfield_square(renderer, t->playfield[y][x], x, y, false);
		}
	}

	render_current(renderer, t);
	render_ghost(renderer, t);
}

void tetris_render(SDL_Renderer *renderer, const struct tetris *t)
{
	SDL_RenderSetViewport(renderer, &hold_viewport);
	render_hold(renderer, t);

	SDL_RenderSetViewport(renderer, &playfield_viewport);
	render_playfield(renderer, t);

	SDL_RenderSetViewport(renderer, &queue_viewport);
	render_queue(renderer, t);
}
