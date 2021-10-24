#ifndef __HAVE_TETRIS_H
#define __HAVE_TETRIS_H

#include <stdbool.h>
#include <stdint.h>

#define PLAYFIELD_WIDTH (10)
#define BUFFERZONE_HEIGHT (4)
#define VISIBLE_PLAYFIELD_HEIGHT (20)
#define PLAYFIELD_HEIGHT (VISIBLE_PLAYFIELD_HEIGHT + BUFFERZONE_HEIGHT)
#define PLAYFIELD_VISIBLE_START (BUFFERZONE_HEIGHT)
#define PLAYFIELD_SPAWN_HEIGHT (PLAYFIELD_VISIBLE_START - 2)

#define DEFAULT_TICK_INTERVAL (100)
#define FAST_TICK_INTERVAL (DEFAULT_TICK_INTERVAL / 4)

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

enum xdirection {
	XDIRECTION_LEFT = -1,
	XDIRECTION_RIGHT = 1,
};

enum rotation_dir {
	ROTATION_CW,
	ROTATION_CCW,
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

	bool gameover;

	bool can_hold;
	bool holding;
	struct tetromino held_tetromino;
};

void tetris_init(struct tetris *t);

void tetris_tick(struct tetris *t);

void tetris_move_current(struct tetris *t, enum xdirection dir);

void tetris_slam(struct tetris *t);

int tetris_slammed_y(struct tetris *t);

void tetris_rotate_current(struct tetris *t, enum rotation_dir dir);

void tetris_hold(struct tetris *t);

int tetromino_min_x(struct tetromino *const tm);

int tetromino_min_y(struct tetromino *const tm);

int tetromino_max_x(struct tetromino *const tm);

int tetromino_max_y(struct tetromino *const tm);

#endif
