#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "tetris.h"

enum square shape_color(enum shape s)
{
	return s + 1;
}

void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
	t->tick_interval = DEFAULT_TICK_INTERVAL;
}

void tetris_tick(struct tetris *t)
{

}

void tetromino_init(struct tetromino *tm, enum shape shape)
{
	tm->shape = shape;
	memset(tm->squares, SQUARE_EMPTY, SHAPE_BOUNDING_BOX_SIZE * SHAPE_BOUNDING_BOX_SIZE);
	enum square color = shape_color(shape);
	switch (shape) {
	case SHAPE_I:
		for (int i = 0; i <= 4; i++)
			tm->squares[1][i] = color;
		break;
	case SHAPE_J:
		break;
	case SHAPE_L:
		break;
	case SHAPE_O:
		break;
	case SHAPE_S:
		break;
	case SHAPE_T:
		break;
	case SHAPE_Z:
		break;
	default:
		break;
	}
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
