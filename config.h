#ifndef __HAVE_CONFIG_H
#define __HAVE_CONFIG_H

#define PLAYFIELD_WIDTH (10)
#define VISIBLE_PLAYFIELD_HEIGHT (20)

#define DEFAULT_TICK_INTERVAL (100)
#define FAST_TICK_INTERVAL (DEFAULT_TICK_INTERVAL / 4)

#define QUEUE_LENGTH (3)

#define ENABLE_DISCOTIME

#define SQUARE_SIZE (30)

#define VIEWPORT_QUEUE_WIDTH (250)
#define VIEWPORT_HOLD_WIDTH (250)

#define GHOST_OPACITY (100)

#define EMPTY_SQUARE_COLOR (0x000000)
#define CYAN               (0x02F8FC)
#define BLUE               (0x1F33E8)
#define ORANGE             (0xEFA00E)
#define YELLOW             (0xF7EB0C)
#define GREEN              (0x2BA320)
#define RED                (0xE23222)
#define PURPLE             (0x780BB7)

#define GAMEOVER_COLOR     (0xFF0A0C)
#define SIDEBARS_BG_COLOR  (0x373737)
#define PIECEBOX_BG_COLOR  (0x000000)

#define I_PIECE_FILL SQUARE_CYAN
#define J_PIECE_FILL SQUARE_BLUE
#define L_PIECE_FILL SQUARE_ORANGE
#define O_PIECE_FILL SQUARE_YELLOW
#define S_PIECE_FILL SQUARE_GREEN
#define T_PIECE_FILL SQUARE_PURPLE
#define Z_PIECE_FILL SQUARE_RED

#endif
