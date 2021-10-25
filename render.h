#ifndef __HAVE_RENDER_H
#define __HAVE_RENDER_H

#include "tetris.h"
#include "config.h"

#define VIEWPORT_PLAYFIELD_WIDTH (SQUARE_SIZE * PLAYFIELD_WIDTH)

#define WINDOW_WIDTH (VIEWPORT_HOLD_WIDTH + VIEWPORT_PLAYFIELD_WIDTH + VIEWPORT_QUEUE_WIDTH)
#define WINDOW_HEIGHT (SQUARE_SIZE * VISIBLE_PLAYFIELD_HEIGHT)

#define SHAPE_BOUNDS_PX (SQUARE_SIZE * SHAPE_BOUNDING_BOX_SIZE)

extern bool discotime;

void tetris_render(SDL_Renderer *renderer, const struct tetris *t);

#endif
