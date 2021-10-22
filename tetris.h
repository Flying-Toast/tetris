#ifndef __HAVE_TETRIS_H
#define __HAVE_TETRIS_H

#include <stdbool.h>
#include <stdint.h>

#define PLAYFIELD_WIDTH (10)
#define PLAYFIELD_HEIGHT (25)
#define PLAYFIELD_VISIBLE_HEIGHT (20)

#define DEFAULT_TICK_INTERVAL (100)

enum square {
	SQUARE_EMPTY = 0,
	SQUARE_CYAN,
	SQUARE_BLUE,
	SQUARE_ORANGE,
	SQUARE_YELLOW,
	SQUARE_GREEN,
	SQUARE_RED,
	SQUARE_PURPLE
};

struct tetris {
	enum square playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];

	//struct piece bag[NUM_PIECE_KINDS];
	int bag_remaining;

	uint32_t tick_interval;
};

struct piece tetris_bag_next(struct tetris *t);

void tetris_init(struct tetris *t);

void tetris_tick(struct tetris *t);

#endif
