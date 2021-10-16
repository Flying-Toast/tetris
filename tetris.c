#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "tetris.h"

static const struct piece all_pieces[NUM_PIECE_KINDS] = {
	// I
	{
		.squares = {
			{SQUARE_EMPTY, SQUARE_EMPTY, SQUARE_EMPTY, SQUARE_EMPTY},
			{SQUARE_CYAN,  SQUARE_CYAN,  SQUARE_CYAN,  SQUARE_CYAN}
		}
	},
	// J
	{
		.squares = {
			{SQUARE_BLUE, SQUARE_EMPTY, SQUARE_EMPTY, SQUARE_EMPTY},
			{SQUARE_BLUE, SQUARE_BLUE,  SQUARE_BLUE,  SQUARE_EMPTY}
		}
	},

	// L
	{
		.squares = {
			{SQUARE_EMPTY,  SQUARE_EMPTY,  SQUARE_ORANGE, SQUARE_EMPTY},
			{SQUARE_ORANGE, SQUARE_ORANGE, SQUARE_ORANGE, SQUARE_EMPTY}
		}
	},

	// O
	{
		.squares = {
			{SQUARE_EMPTY, SQUARE_YELLOW, SQUARE_YELLOW, SQUARE_EMPTY},
			{SQUARE_EMPTY, SQUARE_YELLOW, SQUARE_YELLOW, SQUARE_EMPTY}
		}
	},

	// S
	{
		.squares = {
			{SQUARE_EMPTY, SQUARE_GREEN, SQUARE_GREEN, SQUARE_EMPTY},
			{SQUARE_GREEN, SQUARE_GREEN, SQUARE_EMPTY, SQUARE_EMPTY}
		}
	},

	// Z
	{
		.squares = {
			{SQUARE_RED,   SQUARE_RED, SQUARE_EMPTY, SQUARE_EMPTY},
			{SQUARE_EMPTY, SQUARE_RED, SQUARE_RED,   SQUARE_EMPTY}
		}
	},

	// T
	{
		.squares = {
			{SQUARE_EMPTY,  SQUARE_PURPLE, SQUARE_EMPTY,  SQUARE_EMPTY},
			{SQUARE_PURPLE, SQUARE_PURPLE, SQUARE_PURPLE, SQUARE_EMPTY}
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
	t->curr_piece_loc = struct coord { 4, VISIBLE_PLAYFIELD_HEIGHT + 1 };
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
