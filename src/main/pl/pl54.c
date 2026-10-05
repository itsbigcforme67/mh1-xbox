/* Player code (SLPM_654.95 0x00151220-0x00151348): pl_chr_set family (animation slot setup) */
#include "pl.h"
#include "game.h"
#include "plf.h"
void pad_timer_calc_sub(PLW *pl, int mask);
f32 GetGroundHit(f32 *);
extern u16 for_pad_timer_tbl[4];

static void pl_chr_set_com(PLW *pl, int c, int blend, int tm, int slot) {
    (&pl->char0)[slot] = c;
    (&pl->blend0)[slot] = blend / 2;
    (&pl->act_tm0)[slot] = tm;
    pl->x2FC[slot] = 0;
}

void pl_chr_set(PLW *pl, int c, int blend, int tm, int slot) {
    pl->work81D = 0;
    pl_chr_set_com(pl, c, blend, tm, slot);
}

void pl_chr_set2(PLW *pl, int c, int blend, int tm) {
    pl->work81D = 0;
    pl_chr_set_com(pl, c, blend, tm, 0);
    pl_chr_set_com(pl, c + 100, blend, tm, 1);
}

void pl_chr_set3(PLW *pl, s16 c, int blend, u16 tm, int slot) {
    if (slot < (int)pl->work300) {
        cpRotMatrix((s32 *)pl->ang, (f32 *)((u8 *)pl + 0x20));
        (&pl->char0)[slot] = c;
        (&pl->blend0)[slot] = blend / 2;
        (&pl->act_tm0)[slot] = tm;
        frame_init(pl, (&pl->act_tm0)[slot], (&pl->blend0)[slot], slot);
    }
}
