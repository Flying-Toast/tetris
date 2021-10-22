#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tetris.h"

static const enum square shape_colors[NUM_SHAPES] = {
	[SHAPE_I] = SQUARE_CYAN,
	[SHAPE_J] = SQUARE_BLUE,
	[SHAPE_L] = SQUARE_ORANGE,
	[SHAPE_O] = SQUARE_YELLOW,
	[SHAPE_S] = SQUARE_GREEN,
	[SHAPE_T] = SQUARE_PURPLE,
	[SHAPE_Z] = SQUARE_RED
};

enum square shape_color(enum shape s)
{
	return shape_colors[s];
}

void tetromino_init(struct tetromino *tm, enum shape shape)
{
	tm->shape = shape;
	memset(tm->squares, SQUARE_EMPTY, sizeof(enum square) * SHAPE_BOUNDING_BOX_SIZE * SHAPE_BOUNDING_BOX_SIZE);
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

void tetris_spawn_piece(struct tetris *t)
{
	tetris_bag_next(t, &t->current_tetromino);
	t->current_x = SHAPE_BOUNDING_BOX_SIZE / 2 + 1;
	t->current_y = 0;
}

void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
	t->tick_interval = DEFAULT_TICK_INTERVAL;
	tetris_spawn_piece(t);
}

void tetris_blit_current(struct tetris *t)
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

bool tetris_invalid_current_pos(struct tetris *const t)
{
	for (int y = 0; y < SHAPE_BOUNDING_BOX_SIZE; y++) {
		for (int x = 0; x < SHAPE_BOUNDING_BOX_SIZE; x++) {
			int cx = t->current_x + x;
			int cy = t->current_y + y;
			enum square curr = t->current_tetromino.squares[y][x];

			if (curr == SQUARE_EMPTY)
				continue;

			if (cy < 0 || cy > PLAYFIELD_VISIBLE_HEIGHT - 1)
				return true;

			if (cx < 0 || cx > PLAYFIELD_WIDTH - 1)
				return true;

			if (t->playfield[cy][cx] != SQUARE_EMPTY)
				return true;
		}
	}
	return false;
}

void tetris_tick(struct tetris *t)
{
	t->current_y++;
	if (tetris_invalid_current_pos(t)) {
		t->current_y--;
		tetris_blit_current(t);
		tetris_spawn_piece(t);
	}
}

void tetris_move_current_left(struct tetris *t)
{
	t->current_x--;
	if (tetris_invalid_current_pos(t))
		t->current_x++;
}

void tetris_move_current_right(struct tetris *t)
{
	t->current_x++;
	if (tetris_invalid_current_pos(t))
		t->current_x--;
}

void tetris_bag_next(struct tetris *t, struct tetromino *ret)
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
