/* Player: return to the normal state, part 1 of the object file at
 * SLPM_654.95 0x0014F030-0x0014F850 (inferred). Matches 0x0014F030-0x0014F0C4.
 * normal_char_set and to_normal (0x0014F0D0-0x0014F330) stay in asm for now:
 * see pl_normal_nm.c. In the original these files are one, and pl_st_set,
 * to_normal_fly, to_normal and pl_to_normal_clr_etc are static. */
#include "pl.h"
#include "game.h"
#include "shell.h"


void pl_flag_set(PLW *, int);

void pl_st_set(PLW *pl, s8 st) {
    pl->st = st;
}

/* Despite living in a player file these act on shells (SHLW +0x78). */
int shell_flag_ck(SHLW *sh, int flag) {
    return sh->flag & flag;
}

int shell_flag_set(SHLW *sh, int flag) {
    int ret = sh->flag & flag;
    sh->flag |= flag;
    return ret;
}

void to_normal_fly(PLW *pl, s16 blend, int b) {
    pl->char0 = 0x3F9;
    if (b == 0) {
        pl->work2F4 = 2;
    } else {
        pl->work2F4 = 6;
    }
    pl->blend0 = blend / 2;
    pl->blend1 = blend / 2;
    pl->act_tm0 = 8;
    pl->act_tm1 = 8;
    pl_flag_set(pl, 1);
}
