/* Player code (SLPM_654.95 0x0013C870-0x0013D5E4): pl_mv028 (kick/dodge), pl_mv030, pl_mv031/032 (run and dash with stamina drain), pl_mv033 (action handlers) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv028(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work39C = 0;
        pl->work40C = 6;
        pl->work08 = 0;
        Pl_view_reset(pl);
        pl->x43E = 0;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        t = pl->work56B;
        if (t & 0xF) {
            pl->work56B = t & 0xF0;
            func_549200(pl, 4);
        }
        if (arg1 == 0) {
            pl->x05 = 2;
            if ((pl->kind == 2) && (pl->flag12 != 0)) {
                pl_chr_set2(pl, 0x3F3, 2, 0);
                break;
            }
            pl_chr_set2(pl, 0x1C, 2, 0);
            break;
        }
        pl->x05++;
        pl_chr_set2(pl, 0x16, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x1C, 0, 6);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (arg1 == 0) && (pl->flag12 != 0)) {
            switch (pl->kind) {
            case 4:
                if ((pl->sw.an_trg & 0x20) && (frame_check3(10.0f, 44.0f, pl, 0) != 0) && (pl->x06 == 0)) {
                    pl->x06 = 1;
                    if (pl->sw.an_now & 0x800) {
                        pl->x06 = 2;
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->x06 = 3;
                    }
                }
                if (frame_check2(44.0f, pl, 0) != 0) {
                    t = pl->x06;
                    if (t != 0) {
                        if ((t & 0xFF) == 2) {
                            pl->ang[1] = pl->ang[1] + 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        if (pl->x06 == 3) {
                            pl->ang[1] = pl->ang[1] - 0x2000;
                            pl->ang_y = pl->ang[1];
                        }
                        Pl_act_set2(pl, 1, 0x49, 0xC);
                    }
                }
                break;
            case 2:
                if ((frame_check2(40.0f, pl, 0) != 0) && (pl->sw.now & 0x80)) {
                    Pl_act_set(pl, 1, 0x18, 0);
                }
                break;
            }
        }
        break;
    }
}

void pl_mv030(PLW *pl) {
    f32 sp7C;
    s32 sp70[3];
    f32 sp60[3];
    f32 sp50[3];
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl_chr_set2(pl, 0x12, 0, 0x58);
        pl->ang_y = pl->ang[1];
        sp70[0] = 0;
        sp70[1] = 0;
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        sp60[0] = pl->pos[0] + sp50[0];
        sp60[1] = pl->pos[1] + sp50[1];
        sp60[2] = pl->pos[2] + sp50[2];
        if (GetGroundHitAreaUpper(pl, sp60, &sp7C) == 1) {
            pl->pos[1] = sp7C;
        }
        sp70[2] = 0x425C0000;
        flvecApplyMat33(sp50, (f32 *)sp70, (f32 *)pl->rot);
        pl->pos[0] = pl->pos[0] + sp50[0];
        pl->pos[1] = pl->pos[1] + sp50[1];
        pl->pos[2] = pl->pos[2] + sp50[2];
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv031(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;
    s32 p;
    s16 st;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        pl->work760 = 0x1E;
        switch (arg1) {
        case 0:
            if (pl->char0 != 3) {
                pl_chr_set2(pl, 3, 0, 0);
            }
            break;
        case 1:
            pl_chr_set2(pl, 3, 4, 0x1E);
            break;
        }
        pl->chr_spd0 = 2.5f;
        pl->chr_spd1 = 2.5f;
        pl->ang_y = pl->ang[1];
        break;
    case 1:
        pl->chr_spd0 = 2.5f;
        pl->chr_spd1 = 2.5f;
        Pl_stamina_calc(pl, -1);
        st = pl->stamina;
        if (st < 0x4C) {
            Pl_act_set2(pl, 0, 0x22, 0);
            break;
        }
        if ((pl->sw.trg & 0x40) && (st >= 0x4B) && (Game_clear_ck(1) == 0)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 0, 0x1C, 4);
            break;
        }
        if (pl->sw.pow[0] >= 0x28) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if (front_land_ck2(85.0f, -60.0f, pl, 0) != 0) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 0, 6, 0);
            break;
        }
        if ((pl->work81E != 0) && ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) > 0x4000)) {
            Pl_act_set(pl, 0, 0x20, 0);
            break;
        }
        sp40[2] = 5.0f;
        sp40[0] = 0.0f;
        sp40[1] = 0.0f;
        flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp30[0];
        pl->pos[2] = pl->pos[2] + sp30[2];
        p = stick_pow_get(pl, 0) & 0xFF;
        if ((p != 5) && ((Pl_master_ck(pl) == 1) || (Online_ck() == 0))) {
            if (act_ck(pl, 0, 0x22) == 0 || pl->stamina > 0) {
                if (p == 3) {
                    if (pl->work760 == 0) {
                        Pl_act_set(pl, 0, 0x24, 0);
                    } else {
                        basic_com_ck(pl);
                    }
                } else {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                }
            } else {
                Pl_act_set(pl, 0, 0x21, 0);
            }
        } else {
            basic_com_ck(pl);
        }
        if (pl->x714 != 0) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        break;
    }
}

void pl_mv032(PLW *pl, s32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    s32 p;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x26, 0, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x27, 0, 0);
            break;
        case 2:
            pl_chr_set2(pl, 0x26, 4, 0);
            break;
        }
        pl->ang_y = pl->ang[1];
        break;
    case 1:
        if (arg1 != 1) {
            Pl_stamina_calc(pl, -2);
            if (pl->stamina < 0x4C) {
                Pl_act_set2(pl, 0, 0x22, 0);
                break;
            }
            if (pl->work81E != 0) {
                if ((((pl->x3A8 - pl->ang[1]) + 0x2000) & 0xFFFF) < 0x4001) {
                    goto b22;
                }
                goto b25;
            }
b22:
            Pl_act_set(pl, 0, 0x46, 0);
            break;
        }
        Pl_stamina_calc(pl, -1);
b25:
        if ((pl->sw.trg & 0x40) && (arg1 != 1) && (pl->stamina >= 0x4B)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set(pl, 0, 0x44, 4);
            break;
        }
        if (pl->sw.pow[0] >= 0x28) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if ((front_land_ck2(85.0f, -60.0f, pl, 0) != 0) && (arg1 != 1)) {
            pl->ang_y = pl->ang[1];
            Pl_act_set2(pl, 0, 6, 0);
            break;
        }
        sp50[2] = 1.5f;
        sp50[0] = 0.0f;
        sp50[1] = 0.0f;
        flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
        pl->pos[0] = pl->pos[0] + sp40[0];
        pl->pos[2] = pl->pos[2] + sp40[2];
        p = stick_pow_get(pl, 0) & 0xFF;
        if ((p != 5) && ((Pl_master_ck(pl) == 1) || (Online_ck() == 0))) {
            if (act_ck(pl, 0, 0x22) == 0 || pl->stamina > 0) {
                if ((p == 3) && (arg1 != 1)) {
                    Pl_act_set(pl, 0, 0x24, 0);
                } else {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                }
            } else {
                Pl_act_set(pl, 0, 0x21, 0);
            }
        } else {
            basic_com_ck(pl);
        }
        if (pl->x714 != 0) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        break;
    }
}

void pl_mv033(PLW *pl, s32 arg1) {
    u8 s;
    s32 w;

    Pl_stamina_calc(pl, 4);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->flag12 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        if (arg1 == 0) {
            pl_chr_set2(pl, 5, 4, 0);
            pl->x05 = 2;
            pl->work08 = 0x96;
            break;
        }
        pl_chr_set2(pl, 0x15, 8, 0);
        pl->work08 = 10;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 5, 4, 0x90);
        }
        break;
    case 2:
        w = pl->work08 - 1;
        pl->work08 = w;
        if (w <= 0) {
            pl->x05++;
            pl_chr_set2(pl, 6, 4, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            Pl_stamina_calc(pl, pl->work882);
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
