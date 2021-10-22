#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "tetris.h"

enum square shape_color(enum shape s)
{
	return s + 1;
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

void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
	t->tick_interval = DEFAULT_TICK_INTERVAL;
	t->current_tetromino = tetris_bag_next(t);
	t->current_y = 0;
}

void tetris_tick(struct tetris *t)
{
	t->current_y += 1;
}

struct tetromino tetris_bag_next(struct tetris *t)
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

	return t->bag[--(t->bag_remaining)];
}
