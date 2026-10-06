/* plx04 - stick_pow_get (SLPM_654.95 0x00136D40-0x00136E6C): strength class of the stick (0 none, 1 half, 3 full, 5 = pressed with a
   button; guess from use) for a player. Returns u8 with a masked test `(r & 0xFF)` (the original masks only the test). plf.h declares it
   s32, so the name is renamed around the include (no header edit). Whole file in pl_nm.c. Jump table in main:rodata. */
#define stick_pow_get stick_pow_get_proto_unused
#include "pl.h"
#include "plf.h"
#include "game.h"
#include "plst.h"
#undef stick_pow_get
u8 stick_pow_get(PLW *pl, int arg1) { /* arg1 unused, callers pass 0 */
    u8 r = 0;
    u16 p;
    switch (pl->kind) {
    case 1:
    case 5:
        if (pl->flag12 != 0 && pl->pch_on == 1) return 0;
    default:
    case 0:
    case 2:
    case 3:
    case 4:
        pl->x8C8 = 0;
        p = pl->sw.pow[0];
        if (p >= 0x78) {
            r = 3;
        } else if (p >= 0x55) {
            r = 1;
        } else if (p >= 0x28) {
            r = 1;
        }
        if (pl->st == 0 && pl->flag12 == 0 && (pl->sw.now & 0x10) && (r & 0xFF) && (act_ck(pl, 0, 0x24) == 0 || pl->work760 == 0)) return 5;
        return r;
    }
}
