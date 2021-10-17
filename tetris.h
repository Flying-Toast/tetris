#ifndef __HAVE_TETRIS_H
#define __HAVE_TETRIS_H

#include <stdbool.h>

#define NUM_PIECE_KINDS 7
#define PLAYFIELD_WIDTH 10
#define PLAYFIELD_HEIGHT 25
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

struct coord {
	int x;
	int y;
};

struct floatingsquare {
	enum square sq;
	struct coord coords;
};

struct piece {
	struct floatingsquare squares[2][4];
};

struct tetris {
	enum square playfield[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];

	struct piece bag[NUM_PIECE_KINDS];
	int bag_remaining;

	struct piece curr_piece;
};

struct piece tetris_bag_next(struct tetris *t);

#endif
