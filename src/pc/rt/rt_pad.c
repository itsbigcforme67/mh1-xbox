/*
 * rt_pad.c - host pad backend: fills the game's raw pad state (Psw, main
 * 0x3F3710) the way ioRead_sub (0x11FAC0) does on the PS2, then runs the
 * decompiled swset() (src/main/pad/pad_get.c), which copies it into
 * Plsw_buff / Plan_* for sw_set_sub (pl_normal2.c) to give each player.
 *
 * The host gives the pad as Capcom's fl pad bits plus two sticks; the
 * mapping fl -> game bits is ioRead_sub's. fl pad bit meanings come from
 * ps2pad_hard_to_soft_ds2 (0x306500) read with libpad's byte order (first
 * byte high): 0x1 up, 0x2 down, 0x4 left, 0x8 right, 0x10 cross,
 * 0x20 circle, 0x40 R2, 0x80 L2, 0x100 square, 0x200 triangle, 0x400 R1,
 * 0x800 L1, 0x1000 L3, 0x2000 R3, 0x4000 select, 0x8000 start
 * [inferred from the table; not checked on hardware].
 *
 * Resulting game bits (Psw.sw): 0x2000 up, 0x1000 down, 0x800 left,
 * 0x400 right, 0x200 square, 0x100 triangle, 0x80 R1, 0x40 cross,
 * 0x20 circle, 0x10 R2, 0x8 L1, 0x4 L2, 0x2 L3, 0x1 R3, 0x4000 select,
 * 0x8000 start. Stick angle: flupdate_pad_stick_dir's atan2(-y, x), so 0 =
 * right and 0x4000 = up (y grows downwards, as PS2 sticks do); power
 * 0..127 with ioRead's dead zone (pad_pow_fix: below 45 counts as 0).
 */
#include "rt.h"
#include "types.h"
#include "game.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <string.h>

typedef struct PSW {
    u16 sw;         /* 0x00 */
    u16 old;        /* 0x02 */
    u16 trg;        /* 0x04 pressed this tick */
    u16 rel;        /* 0x06 released this tick */
    u16 an_sw;      /* 0x08 stick directions as bits */
    u16 an_old;     /* 0x0A */
    u16 an_trg;     /* 0x0C */
    u16 an_rel;     /* 0x0E */
    u16 ang[2];     /* 0x10 left, right stick angle */
    u16 pow[2];     /* 0x14 */
    u16 rep;        /* 0x18 key repeat */
    u16 rep_cnt;    /* 0x1A */
    u16 on;         /* 0x1C pad connected */
    u16 on_old;     /* 0x1E */
    u16 x20;        /* 0x20 */
} PSW;

PSW Psw[4];
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

static const u16 fl_to_game[16] = {
    0x2000, 0x1000, 0x800, 0x400, 0x40, 0x20, 0x10, 0x4,
    0x200, 0x100, 0x80, 0x8, 0x2, 0x1, 0x4000, 0x8000,
};

static u16 pad_pow_fix(int p) { return (u16)((unsigned)p < 45 ? 0 : p); }

static void stick(int x, int y, u16 *ang, u16 *pow)
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
    *ang = (u16)(((int)(r / (2 * M_PI) * 360.0) << 16) / 360);
    p = (int)sqrt((double)x * x + (double)y * y);
    *pow = pad_pow_fix(p > 127 ? 127 : p);
}

/* ioRead_sub for port 0 with the host's input; ports 1-3 stay unplugged. */
static void read_port0(void)
{
    PSW *p = &Psw[0];
    u16 sw = 0, an = 0;
    int i;
    p->on_old = p->on;
    p->on = 1;
    p->x20 = 0;
    p->old = p->sw;
    p->an_old = p->an_sw;
    for (i = 0; i < 16; i++)
        if (host_bits & (1 << i))
            sw |= fl_to_game[i];
    p->sw = sw;
    /* stick direction bits (fl bits 16-23): beyond half way [guess] */
    if (host_stick[1] < -63) an |= 0x2000;
    if (host_stick[1] > 63) an |= 0x1000;
    if (host_stick[0] < -63) an |= 0x800;
    if (host_stick[0] > 63) an |= 0x400;
    if (host_stick[3] < -63) an |= 0x20;
    if (host_stick[3] > 63) an |= 0x10;
    if (host_stick[2] < -63) an |= 0x8;
    if (host_stick[2] > 63) an |= 0x4;
    stick(host_stick[0], host_stick[1], &p->ang[0], &p->pow[0]);
    stick(host_stick[2], host_stick[3], &p->ang[1], &p->pow[1]);
    p->an_sw = an;
    p->trg = p->sw & ~p->old;
    p->rel = p->old & ~p->sw;
    p->an_trg = p->an_sw & ~p->an_old;
    p->an_rel = p->an_old & ~p->an_sw;
    p->rep = 0;
    if (p->old == p->sw) {
        p->rep_cnt++;
        if (p->rep_cnt >= 0xB && p->rep_cnt >= 0xD) {
            p->rep_cnt = 0xA;
            p->rep = p->sw;
        }
    } else {
        p->rep_cnt = 0;
    }
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
