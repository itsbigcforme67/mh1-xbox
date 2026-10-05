/* Player code (SLPM_654.95 0x00148490-0x00148780): pl_dm007: damage reaction handler (knock-back jump) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_dm007(PLW *pl, s32 arg1) {
    f32 spA0[3];
    f32 sp90[3];
    s32 sp80[3];
    f32 sp40[16];
    f32 v;
    u16 c;
    u8 s;

    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0xD5, 0, 0);
        } else {
            pl_chr_set2(pl, 0xDB, 0, 0);
        }
        get_joint_pos(pl, 2, spA0);
        pl->pos[1] = spA0[1];
        Pl_basic_flagset(pl, 2, 0, 1);
        pl->flag604 = 0;
        rate_clear_g(pl);
        sp80[0] = 0;
        sp80[1] = pl->ang[1];
        sp80[2] = 0;
        cpRotMatrix(sp80, sp40);
        spA0[0] = 0.0f;
        spA0[1] = 0.0f;
        if (arg1 != 0) {
            v = 7.0f;
        } else {
            v = -7.0f;
        }
        spA0[2] = v;
        flvecApplyMat33(sp90, spA0, sp40);
        pl->vel[0] = 2.0f * sp90[0];
        pl->vel[2] = 2.0f * sp90[2];
        pl->vel[1] = 20.0f;
        pl->acc[1] = -0.72727275f;
        break;
    case 1:
        if (pl->work194 == 0) {
            c = pl->char0;
            if (c != 0xD5) {
                if (c == 0xDB) {
                    goto go;
                }
            } else {
go:
                if (arg1 == 0) {
                    pl_chr_set2(pl, 0xD6, 0, 0);
                } else {
                    pl_chr_set2(pl, 0xDC, 0, 0);
                }
            }
        }
        rate_add_g(pl);
        if ((pl->vel[1] <= 0.0f) && (pl->pos[1] <= (63.0f + pl->x5AC))) {
            pl->x05++;
            Pl_se_req2_com(pl, 0x42, 0, pl->pos, 1, 0);
            armor_sd_req(pl, 3);
            pl_chr_set2(pl, 0xD7, 0, 0);
            pl->pos[1] = pl->x5AC;
            pl->st = 3;
            if (game_w.stage == 0) {
                func_546860(pl, pl->pos, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->ang[1] = (pl->ang[1] + 0x8000) & 0xFFFF;
            pl->ang_y = pl->ang[1];
            pl_chr_set2(pl, 0xD8, 0, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            pl->work40C = 6;
        }
        break;
    }
}
