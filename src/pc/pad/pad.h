/*
 * pad.h - the port's controller interface (platform side). A pad state is
 * Capcom's fl pad bits plus two sticks; src/pc/rt/rt_pad.c turns it into
 * the game's Psw like the PS2 pad driver (ioRead_sub) does.
 *
 * fl bits: 0x1 up, 0x2 down, 0x4 left, 0x8 right, 0x10 cross, 0x20 circle,
 * 0x40 R2, 0x80 L2, 0x100 square, 0x200 triangle, 0x400 R1, 0x800 L1,
 * 0x1000 L3, 0x2000 R3, 0x4000 select, 0x8000 start.
 * Sticks -127..127, y grows downwards (PS2 convention).
 */
#ifndef MH_PAD_H
#define MH_PAD_H

#include <stdint.h>

typedef struct {
    uint16_t bits;
    int lx, ly, rx, ry;
} pad_state;

enum {
    PAD_UP = 0x1, PAD_DOWN = 0x2, PAD_LEFT = 0x4, PAD_RIGHT = 0x8,
    PAD_CROSS = 0x10, PAD_CIRCLE = 0x20, PAD_R2 = 0x40, PAD_L2 = 0x80,
    PAD_SQUARE = 0x100, PAD_TRIANGLE = 0x200, PAD_R1 = 0x400, PAD_L1 = 0x800,
    PAD_L3 = 0x1000, PAD_R3 = 0x2000, PAD_SELECT = 0x4000, PAD_START = 0x8000
};

/* Open the first SDL game controller if there is one (hot-plug handled). */
void pad_init(void);
/* Current state: game controller merged with the keyboard (keyboard: 1 to
 * use it; W/A/S/D left stick, arrows right stick, K cross, L circle,
 * J square, I triangle, Q L1, E R1, Z L2, C R2, Enter start, Backspace
 * select, T/F/G/H d-pad). */
void pad_read(pad_state *p, int keyboard);

/* Scripted input for offscreen tests: "name+name*ticks,..." with names
 * up/down/left/right (left stick), cam_l/cam_r/cam_u/cam_d (right stick),
 * cross, circle, square, triangle, l1, r1, l2, r2, l3, r3, start, select,
 * dup, ddown, dleft, dright, idle. Returns 0 on a parse error. */
int pad_script_set(const char *script);
/* Next tick of the script into p; 0 once the script has ended (p idle). */
int pad_script_next(pad_state *p);

#endif
