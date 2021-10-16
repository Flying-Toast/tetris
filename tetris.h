#ifndef __HAVE_TETRIS_H
#define __HAVE_TETRIS_H

#include <stdbool.h>

#define NUM_PIECE_KINDS 7
#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 22
#define VISIBLE_PLAYFIELD_HEIGHT 20

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

struct piece {
	enum square squares[2][4];
};

struct tetris {
	enum square playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
	struct piece bag[NUM_PIECE_KINDS];
	int bag_remaining;
	struct piece hold;
	bool holding;
	bool hold_lock;
};

#endif
