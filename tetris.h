#ifndef __HAVE_TETRIS_H
#define __HAVE_TETRIS_H

#include <stdbool.h>
#include <stdint.h>

#define PLAYFIELD_WIDTH (10)
#define PLAYFIELD_HEIGHT (25)
#define PLAYFIELD_VISIBLE_HEIGHT (20)

#define DEFAULT_TICK_INTERVAL (100)
#define SLAM_TICK_INTERVAL (DEFAULT_TICK_INTERVAL / 4)

#define SHAPE_BOUNDING_BOX_SIZE (4)

enum shape {
	SHAPE_I = 0,
	SHAPE_J,
	SHAPE_L,
	SHAPE_O,
	SHAPE_S,
	SHAPE_T,
	SHAPE_Z,
	NUM_SHAPES
};

enum square {
	SQUARE_EMPTY = 0,
	SQUARE_CYAN,
	SQUARE_BLUE,
	SQUARE_ORANGE,
	SQUARE_YELLOW,
	SQUARE_GREEN,
	SQUARE_PURPLE,
	SQUARE_RED
};

struct tetromino {
	enum shape shape;
	enum square squares[SHAPE_BOUNDING_BOX_SIZE][SHAPE_BOUNDING_BOX_SIZE];
};

struct tetris {
	enum square playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];

	struct tetromino bag[NUM_SHAPES];
	int bag_remaining;

	uint32_t tick_interval;

	struct tetromino current_tetromino;
	int current_y;
	int current_x;
};

void tetris_bag_next(struct tetris *t, struct tetromino *ret);

void tetris_init(struct tetris *t);

void tetris_tick(struct tetris *t);

void tetris_move_current_left(struct tetris *t);
void tetris_move_current_right(struct tetris *t);

#endif
