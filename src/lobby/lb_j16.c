/* lb_j16 - x 0x005D0460-0x005D0684: lb_pl_mv031. Whole file in lb_j.c. */
#include "lobby.h"






void pl_sleeping();





























void lb_pl_mv031(PLW *pl) {
    LBV3 in;
    LBV3 out;
    u8 t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        pl->work760 = 0x1E;
        if (PLU8(pl, 0x8EC) != 0) {
            if (pl->char0 != 0x27) {
                pl_chr_set2(pl, 0x27, 4, 0);
            }
            pl->chr_spd0 = 2.0f;
            pl->chr_spd1 = 2.0f;
        } else if (pl->char0 == 3) {
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
        } else {
            pl_chr_set2(pl, 3, 0, 0x3A);
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
        }
        pl->ang_y = pl->ang[1];
        return;
    case 1:
        if (pl->id == game_w.master && pl->sw.pow[0] >= 0x28) {
            pl->ang_y = Lb_stick_dir_set(pl, 0);
        }
        in.x = 0;
        in.y = 0;
        if (PLU8(pl, 0x8EC) != 0) {
            *(s32 *)&in.z = 0x3F800000;
        } else {
            *(s32 *)&in.z = 0x40A00000;
        }
        flvecApplyMat33(&out, &in, (u8 *)pl + 0x20);
        pl->pos[0] = pl->pos[0] + out.x;
        pl->pos[2] = pl->pos[2] + out.z;
        t = Lb_stick_pow_get(pl);
        if (t != 5 && (game_w.master == pl->id || Online_ck() == 0)) {
            if (t != 0) {
                if (pl->work760 == 0) {
                    Lb_Pl_act_set(pl, 0, 2, 0);
                    return;
                }
                lb_basic_com_ck(pl);
                return;
            }
            Lb_Pl_act_set(pl, 0, 0x2C, 0);
            return;
        }
        lb_basic_com_ck(pl);
    }
}
