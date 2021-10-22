#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "tetris.h"

void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
	t->tick_interval = DEFAULT_TICK_INTERVAL;
}

void tetris_tick(struct tetris *t)
{

}

struct tetromino tetris_bag_next(struct tetris *t)
{
	if (t->bag_remaining == 0) {
		t->bag_remaining = NUM_PIECE_KINDS;
		memcpy(t->bag, all_pieces, sizeof(t->bag));
		for (int i = NUM_PIECE_KINDS - 1; i > 0; i--) {
			double frac = ((double) rand()) / ((double) RAND_MAX);
			int j = round(frac * ((double) i));
			struct tetromino tmp = t->bag[j];
			t->bag[j] = t->bag[i];
			t->bag[i] = tmp;
		}
	}

	return t->bag[--(t->bag_remaining)];
}
