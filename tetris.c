#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tetris.h"

static const enum square shape_colors[] = {
	[SHAPE_I] = SQUARE_CYAN,
	[SHAPE_J] = SQUARE_BLUE,
	[SHAPE_L] = SQUARE_ORANGE,
	[SHAPE_O] = SQUARE_YELLOW,
	[SHAPE_S] = SQUARE_GREEN,
	[SHAPE_T] = SQUARE_PURPLE,
	[SHAPE_Z] = SQUARE_RED
};

static const int cw_3x3_rot[3][3][2] = {
	{{2, 0}, {1, 0}, {0, 0}},
	{{2, 1}, {1, 1}, {0, 1}},
	{{2, 2}, {1, 2}, {0, 2}}
};

static const int cw_4x4_rot[4][4][2] = {
	{{3, 0}, {2, 0}, {1, 0}, {0, 0}},
	{{3, 1}, {2, 1}, {1, 1}, {0, 1}},
	{{3, 2}, {2, 2}, {1, 2}, {0, 2}},
	{{3, 3}, {2, 3}, {1, 3}, {0, 3}}
};

static enum square shape_color(enum shape s)
{
	return shape_colors[s];
}

static int shape_bounds(enum shape s)
{
	if (s == SHAPE_I)
		return 4;
	return 3;
}

static void tetromino_init(struct tetromino *tm, enum shape shape)
{
	tm->shape = shape;
	memset(tm->squares, SQUARE_EMPTY, sizeof(tm->squares));
	enum square color = shape_color(shape);
	switch (shape) {
	case SHAPE_I:
		tm->squares[1][0] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		tm->squares[1][3] = color;
		break;
	case SHAPE_J:
		tm->squares[0][0] = color;
		tm->squares[1][0] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		break;
	case SHAPE_L:
		tm->squares[0][2] = color;
		tm->squares[1][0] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		break;
	case SHAPE_O:
		tm->squares[0][1] = color;
		tm->squares[0][2] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		break;
	case SHAPE_S:
		tm->squares[0][1] = color;
		tm->squares[0][2] = color;
		tm->squares[1][0] = color;
		tm->squares[1][1] = color;
		break;
	case SHAPE_T:
		tm->squares[0][1] = color;
		tm->squares[1][0] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		break;
	case SHAPE_Z:
		tm->squares[0][0] = color;
		tm->squares[0][1] = color;
		tm->squares[1][1] = color;
		tm->squares[1][2] = color;
		break;
	default:
		break;
	}
}

static void tetris_bag_next(struct tetris *t, struct tetromino *ret)
{
	if (t->bag_remaining == 0) {
		t->bag_remaining = NUM_SHAPES;
		for (enum shape i = 0; i < NUM_SHAPES; i++) {
			tetromino_init(&t->bag[i], i);
		}
		for (int i = NUM_SHAPES - 1; i > 0; i--) {
			double frac = ((double) rand()) / ((double) RAND_MAX);
			int j = round(frac * ((double) i));
			struct tetromino tmp = t->bag[j];
			t->bag[j] = t->bag[i];
			t->bag[i] = tmp;
		}
	}

	*ret = t->bag[--(t->bag_remaining)];
}

static void tetris_clear_single_line(struct tetris *t, int line_y)
{
	for (int y = line_y; y > 0; y--) {
		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			t->playfield[y][x] = t->playfield[y - 1][x];
		}
	}

	for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
		t->playfield[0][x] = SQUARE_EMPTY;
	}
}

static void tetris_clear_lines(struct tetris *t)
{
	for (int y = PLAYFIELD_HEIGHT - 1; y >= 0; y--) {
		bool line_full = true;

		for (int x = 0; x < PLAYFIELD_WIDTH; x++) {
			if (t->playfield[y][x] == SQUARE_EMPTY) {
				line_full = false;
				break;
			}
		}

		if (line_full) {
			tetris_clear_single_line(t, y);
			y++; // process this same y level again because we just shifted a new row down into it
		}
	}
}

static void tetris_reset_current_loc(struct tetris *t)
{
	t->current_x = SHAPE_BOUNDING_BOX_SIZE / 2 + 1;
	t->current_y = PLAYFIELD_SPAWN_HEIGHT;
}

int tetris_queue_real_index(struct tetris *t, int idx)
{
	return (t->queue_start + idx) % QUEUE_LENGTH;
}

static void tetris_spawn_piece(struct tetris *t)
{
	t->current_tetromino = t->queue[t->queue_start];
	tetris_bag_next(t, &t->queue[t->queue_start]);
	t->queue_start = (t->queue_start + 1) % QUEUE_LENGTH;
	tetris_reset_current_loc(t);
}

void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
	t->tick_interval = DEFAULT_TICK_INTERVAL;
	t->gameover = false;
	t->can_hold = true;
	t->holding = false;
	t->queue_start = 0;
	for (int i = 0; i < QUEUE_LENGTH; i++)
		tetris_bag_next(t, t->queue + i);
	tetris_spawn_piece(t);
}

static void tetris_blit_current(struct tetris *t)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			enum square curr = t->current_tetromino.squares[y][x];
			if (curr != SQUARE_EMPTY) {
				t->playfield[y + t->current_y][x + t->current_x] = curr;
			}
		}
	}
}

static bool tetris_invalid_current_at_y(struct tetris *const t, int at_y)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			int cx = t->current_x + x;
			int cy = at_y + y;
			enum square curr = t->current_tetromino.squares[y][x];

			if (curr == SQUARE_EMPTY)
				continue;

			if (cy < 0 || cy > PLAYFIELD_HEIGHT - 1)
				return true;

			if (cx < 0 || cx > PLAYFIELD_WIDTH - 1)
				return true;

			if (t->playfield[cy][cx] != SQUARE_EMPTY)
				return true;
		}
	}
	return false;
}

static bool tetris_invalid_current_pos(struct tetris *const t)
{
	return tetris_invalid_current_at_y(t, t->current_y);
}

static bool tetris_move_current_down(struct tetris *t)
{
	t->current_y++;
	if (tetris_invalid_current_pos(t)) {
		t->current_y--;
		return false;
	}
	return true;
}

void tetris_tick(struct tetris *t)
{
	if (t->gameover)
		return;

	if (!tetris_move_current_down(t)) {
		tetris_blit_current(t);
		tetris_spawn_piece(t);
		t->can_hold = true;
		if (tetris_invalid_current_pos(t))
			t->gameover = true;
	}
	tetris_clear_lines(t);
}

void tetris_move_current(struct tetris *t, enum xdirection dir)
{
	t->current_x += dir;
	if (tetris_invalid_current_pos(t))
		t->current_x -= dir;
}

void tetris_slam(struct tetris *t)
{
	if (tetris_invalid_current_pos(t))
		return;

	while (tetris_move_current_down(t))
		;

	tetris_tick(t);
}

int tetris_slammed_y(struct tetris *const t)
{
	int slammed_y = t->current_y;
	while (!tetris_invalid_current_at_y(t, slammed_y))
		slammed_y++;
	return slammed_y - 1;
}

void tetris_rotate_current(struct tetris *t, enum rotation_dir dir)
{
	if (t->current_tetromino.shape == SHAPE_O)
		return;

	enum square tmpsquares[SHAPE_BOUNDING_BOX_SIZE][SHAPE_BOUNDING_BOX_SIZE];
	memcpy(tmpsquares, t->current_tetromino.squares, sizeof(tmpsquares));
	int bounds = shape_bounds(t->current_tetromino.shape);

	for (int y = 0; y < bounds; y++) {
		for (int x = 0; x < bounds; x++) {
			const int *coords;
			if (bounds == 3) {
				coords = cw_3x3_rot[y][x];
			} else {
				coords = cw_4x4_rot[y][x];
			}
			if (dir == ROTATION_CW) {
				t->current_tetromino.squares[y][x] = tmpsquares[coords[0]][coords[1]];
			} else {
				t->current_tetromino.squares[coords[0]][coords[1]] = tmpsquares[y][x];
			}
		}
	}

	if (tetris_invalid_current_pos(t)) {
		memcpy(t->current_tetromino.squares, tmpsquares, sizeof(tmpsquares));
	}
}

void tetris_hold(struct tetris *t)
{
	if (!t->can_hold)
		return;
	t->can_hold = false;

	if (t->holding) {
		struct tetromino tmp = t->current_tetromino;
		t->current_tetromino = t->held_tetromino;
		t->held_tetromino = tmp;
		tetris_reset_current_loc(t);
	} else {
		t->held_tetromino = t->current_tetromino;
		t->holding = true;
		tetris_spawn_piece(t);
	}
}
