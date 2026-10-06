/* Player wall checks (SLPM_654.95 0x00138900-0x00138BE0): wall_act_ck (is the player against a wall, turn to it) and
   wall_vec_set (same for the wall vector). Whole file in pl_nm.c. */
#include "pl.h"
#include "plf.h"
#include "game.h"

u16 calc_vec_ang(f32, f32, f32, f32);

/* near-match: one commutated addu (original: (base+id*252)+off, ours off+(base+id*252)) */
s32 wall_act_ck(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C != 0) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && (pl->work74C & 0xE0000007)) {
                if (mode == 0) {
                    pl->ang[1] = (u16)(ang - 0x4000);
                } else {
                    pl->ang[1] = (u16)(ang + 0x4000);
                }
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (flag != 0) {
                if (mode == 1) {
                    pl->ang[1] = (u16)(ang + 0x4000);
                    pl->ang_y = pl->ang[1];
                }
                return 2;
            }
        }
    }
    return 0;
}

s32 wall_vec_set(PLW *pl, s16 mode) {
    s16 i;
    int off;
    u16 flag;
    u16 ang;
    PL_WALL *w;

    if (!(Pl_stg_ck(pl) & 0xFF)) {
        return 0;
    }
    if (pl->work74C & (mode == 0 ? 0xE0000007 : 0x3E000)) {
        for (i = 0, off = 0; i < 20; i++, off += 12) {
            w = pl_wall_mat[pl->id];
            w = (PL_WALL *)((u8 *)w + off);
            flag = w->flag;
            if (flag == 0) {
                break;
            }
            ang = calc_vec_ang(w->vec[0], w->vec[2], 0.0f, 0.0f);
            if ((flag & 2) && mode == 0) {
                pl->ang[1] = (u16)(ang - 0x4000);
                pl->ang_y = pl->ang[1];
                return 1;
            }
            if (mode == 1) {
                pl->ang[1] = (u16)(ang + 0x4000);
                pl->ang_y = pl->ang[1];
                return 2;
            }
        }
    }
    return 0;
}
