#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "tetris.h"

static const struct piece all_pieces[NUM_PIECE_KINDS] = {
	// I
	{
		.squares = {
			{{SQUARE_EMPTY, {0, 1}}, {SQUARE_EMPTY, {1, 1}}, {SQUARE_EMPTY, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_CYAN,  {0, 0}}, {SQUARE_CYAN,  {1, 0}}, {SQUARE_CYAN,  {2, 0}}, {SQUARE_CYAN,  {3, 0}}}
		}
	},
	// J
	{
		.squares = {
			{{SQUARE_BLUE, {0, 1}}, {SQUARE_EMPTY, {1, 1}}, {SQUARE_EMPTY, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_BLUE, {0, 0}}, {SQUARE_BLUE,  {1, 0}}, {SQUARE_BLUE,  {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	},

	// L
	{
		.squares = {
			{{SQUARE_EMPTY,  {0, 1}}, {SQUARE_EMPTY,  {1, 1}}, {SQUARE_ORANGE, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_ORANGE, {0, 0}}, {SQUARE_ORANGE, {1, 0}}, {SQUARE_ORANGE, {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	},

	// O
	{
		.squares = {
			{{SQUARE_EMPTY, {0, 1}}, {SQUARE_YELLOW, {1, 1}}, {SQUARE_YELLOW, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_EMPTY, {0, 0}}, {SQUARE_YELLOW, {1, 0}}, {SQUARE_YELLOW, {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	},

	// S
	{
		.squares = {
			{{SQUARE_EMPTY, {0, 1}}, {SQUARE_GREEN, {1, 1}}, {SQUARE_GREEN, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_GREEN, {0, 0}}, {SQUARE_GREEN, {1, 0}}, {SQUARE_EMPTY, {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	},

	// Z
	{
		.squares = {
			{{SQUARE_RED,   {0, 1}}, {SQUARE_RED, {1, 1}}, {SQUARE_EMPTY, {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_EMPTY, {0, 0}}, {SQUARE_RED, {1, 0}}, {SQUARE_RED,   {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	},

	// T
	{
		.squares = {
			{{SQUARE_EMPTY,  {0, 1}}, {SQUARE_PURPLE, {1, 1}}, {SQUARE_EMPTY,  {2, 1}}, {SQUARE_EMPTY, {3, 1}}},
			{{SQUARE_PURPLE, {0, 0}}, {SQUARE_PURPLE, {1, 0}}, {SQUARE_PURPLE, {2, 0}}, {SQUARE_EMPTY, {3, 0}}}
		}
	}
};


void tetris_init(struct tetris *t)
{
	t->bag_remaining = 0;
	memset(&t->playfield, SQUARE_EMPTY, sizeof(t->playfield));
}

void tetris_spawn_piece(struct tetris *t)
{
	t->curr_piece = tetris_bag_next(t);
	for (int y = 0; y < PIECE_MAX_HEIGHT; y++) {
		for (int x = 0; x < PIECE_MAX_WIDTH; x++) {
			t->curr_piece.squares[y][x].coords.y += VISIBLE_PLAYFIELD_HEIGHT;
			t->curr_piece.squares[y][x].coords.x += PIECE_LEFT_START;
		}
	}
}

void tetris_start(struct tetris *t)
{
	tetris_spawn_piece(t);
}

void tetris_tick(struct tetris *t)
{

}

struct piece tetris_bag_next(struct tetris *t)
{
	if (t->bag_remaining == 0) {
		t->bag_remaining = NUM_PIECE_KINDS;
		memcpy(t->bag, all_pieces, sizeof(t->bag));
		for (int i = NUM_PIECE_KINDS - 1; i > 0; i--) {
			double frac = ((double) rand()) / ((double) RAND_MAX);
			int j = round(frac * ((double) i));
			struct piece tmp = t->bag[j];
			t->bag[j] = t->bag[i];
			t->bag[i] = tmp;
		}
	}

	return t->bag[--(t->bag_remaining)];
}
