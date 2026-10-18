/*
 * rt_pad.c - host pad backend: plays the PS2 fl pad library (flPADGetALL)
 * for the game's own pad driver. The host fills flpad_adr[0] (the fl
 * library's pad data) from the host pad; the decompiled ioRead / ioRead2 /
 * ioRead_sub (src/main/sys/f_ioread.c, matching the original) turn it into
 * the game's pad words Psw, then the decompiled swset()
 * (src/main/pad/pad_get.c) copies those into Plsw_buff / Plan_* for
 * sw_set_sub (pl_normal2.c) to give each player.
 *
 * The host gives the pad as Capcom's fl pad bits plus two sticks. fl pad
 * bit meanings come from ps2pad_hard_to_soft_ds2 (0x306500) read with
 * libpad's byte order (first byte high): 0x1 up, 0x2 down, 0x4 left,
 * 0x8 right, 0x10 cross, 0x20 circle, 0x40 R2, 0x80 L2, 0x100 square,
 * 0x200 triangle, 0x400 R1, 0x800 L1, 0x1000 L3, 0x2000 R3, 0x4000 select,
 * 0x8000 start [inferred from the table; not checked on hardware].
 *
 * ioRead_sub maps them to the game bits (Psw.on): 0x2000 up, 0x1000 down,
 * 0x800 left, 0x400 right, 0x200 square, 0x100 triangle, 0x80 R1,
 * 0x40 cross, 0x20 circle, 0x10 R2, 0x8 L1, 0x4 L2, 0x2 L3, 0x1 R3,
 * 0x4000 select, 0x8000 start. Stick angle (what the fl library gives):
 * flupdate_pad_stick_dir's atan2(-y, x) in degrees, so 0 = right and 90 =
 * up (y grows downwards, as PS2 sticks do); power 0..127 (ioRead_sub's
 * pad_pow_fix: below 45 counts as 0). The stick direction bits
 * (fl bits 16-23) are set beyond half way [guess].
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "ioread.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <string.h>

u16 Psw[4 * 17];                /* the game's pad words, 0x22 bytes per pad (PSWE, include/ioread.h) */
static FPADI host_fpad[4];      /* the fl library's pad data: port 0 = the host pad, 1-3 unplugged */
FPADI *flpad_adr = host_fpad;
void ioRead(void);              /* src/main/sys/f_ioread.c */
void swset(void);

static u16 host_bits;
static int host_stick[4];   /* lx, ly, rx, ry: -127..127, y down */

/* the PC settings menu's "Western" layout (cross confirms): circle and cross trade places for the whole game, menus and play */
int rt_pad_swap_confirm;

void rt_pad_set(uint16_t fl_bits, int lx, int ly, int rx, int ry)
{
    rt_pick_record_pad(fl_bits, lx, ly, rx, ry);
    if (rt_pad_swap_confirm && ((fl_bits & 0x30) == 0x10 || (fl_bits & 0x30) == 0x20))
        fl_bits ^= 0x30;                        /* 0x10 cross <-> 0x20 circle (one of them held; both held stay as they are) */
    host_bits = fl_bits;
    host_stick[0] = lx;
    host_stick[1] = ly;
    host_stick[2] = rx;
    host_stick[3] = ry;
}

/* test aids (rt_player.c RT_PL_CARVE): change this tick's host buttons, optionally drop the right stick */
void rt_pad_aid(uint16_t clear, uint16_t set, int right_stick_off)
{
    host_bits = (u16)((host_bits & ~clear) | set);
    if (right_stick_off)
        host_stick[2] = host_stick[3] = 0;
}

/* the fl library's stick values: angle in degrees (atan2(-y, x)) and power 0..127 */
static void stick(int x, int y, s16 *ang, s16 *pow)
{
    double r;
    int p;
    if (x == 0 && y == 0) {
        *ang = 0;
        *pow = 0;
        return;
    }
    r = atan2((double)-y, (double)x);
    if (r < 0)
        r += 2 * M_PI;
    /* rounded: truncating gave 269 for a stick held straight down (3/2 pi / 2 pi * 360 is a hair under 270 in
     * double), so the hunter ran 1 degree off; the lockstep referee showed it against PCSX2 (docs/referee.md) */
    *ang = (s16)((int)floor(r / (2 * M_PI) * 360.0 + 0.5) % 360);
    p = (int)sqrt((double)x * x + (double)y * y);
    *pow = (s16)(p > 127 ? 127 : p);
}

/* flPADGetALL for port 0 with the host's input, then the game's ioRead. */
static void read_port0(void)
{
    FPADI *f = &host_fpad[0];
    u32 b = host_bits;
    f->state = 1;
    f->x02 = 1;
    f->mode = 0x70;                             /* both sticks analog */
    if (host_stick[1] < -63) b |= 0x10000;      /* left stick up / down / left / right */
    if (host_stick[1] > 63) b |= 0x20000;
    if (host_stick[0] < -63) b |= 0x40000;
    if (host_stick[0] > 63) b |= 0x80000;
    if (host_stick[3] < -63) b |= 0x100000;     /* right stick */
    if (host_stick[3] > 63) b |= 0x200000;
    if (host_stick[2] < -63) b |= 0x400000;
    if (host_stick[2] > 63) b |= 0x800000;
    f->btn = b;
    stick(host_stick[0], host_stick[1], &f->ang0, &f->pow0);
    stick(host_stick[2], host_stick[3], &f->ang1, &f->pow1);
    ioRead();
}

/* The pad driver only (the village's Lb_pl_move runs swset itself). */
void rt_pad_read(void)
{
    read_port0();
    game_w.pad_on = 1;
    game_w.port[0] = 0;
    game_w.port[1] = 1;
}

/* Once per game tick, before the player code: pad driver + swset(). */
void rt_pad_tick(void)
{
    read_port0();
    game_w.pad_on = 1;
    game_w.port[0] = 0;
    game_w.port[1] = 1;
    swset();
}
