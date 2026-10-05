/* Player code (SLPM_654.95 0x00142300-0x00143420): pl_at000, pl_at001, pl_at004, pl_at006: weapon attack handlers (sword-type chains: arg1 selects the swing, x05/x06/x07 are step/sub-step/hit flag) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_at000(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work615 = 1;
        pl->x07 = 0;
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57E, 4, 0);
            action_timer_calc(pl, 0);
            func_6362B0(pl, 2);
            Pl_basic_flagset(pl, 0, 1, 0);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x589, 4, 0);
                    break;
                case 2:
                    pl_chr_set2(pl, 0x58D, 4, 0);
                    break;
                case 3:
                    pl->ang[1] = (pl->ang[1] + 0x2000) & 0xFFFF;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x58F, 0, 0);
                    break;
                case 4:
                    pl->ang[1] = pl->ang[1] + 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x593, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    switch (arg1) {
                    case 2:
                        pl_chr_set2(pl, 0x57E, 2, 0x2C);
                        break;
                    case 4:
                        pl_chr_set2(pl, 0x57E, 0, 0x2C);
                        break;
                    default:
                        pl_chr_set2(pl, 0x57E, 2, 0x1A);
                        break;
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 2);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                pl->work615 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 2);
                break;
            }
        } else if ((frame_check2(78.0f, pl, 0) != 0) && (frame_check2(104.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 2);
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(78.0f, pl, 0) != 0) && (frame_check2(104.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 2);
        }
        break;
    }
}

void pl_at001(PLW *pl, s32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->work615 = 1;
        pl->x07 = 0;
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57F, 4, 0);
            pl->work39C = 0;
            func_6362B0(pl, 1);
            Pl_basic_flagset(pl, 0, 1, 0);
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x588, 4, 0);
                    break;
                case 2:
                    pl_chr_set2(pl, 0x58B, 4, 0);
                    break;
                case 3:
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x58E, 0, 0);
                    break;
                case 4:
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x591, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    if (arg1 == 1) {
                        pl_chr_set2(pl, 0x57F, 2, 0x40);
                    } else {
                        pl_chr_set2(pl, 0x57F, 0, 0x42);
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 1);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                pl->work615 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 2);
                break;
            }
        } else {
            if ((pl->x07 == 0) && (frame_check(106.0f, pl, 0) != 0)) {
                sp50[0] = 30.0f;
                sp50[1] = 0.0f;
                sp50[2] = -150.0f;
                flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
                sp50[0] = pl->pos[0] + sp40[0];
                sp50[1] = 15.0f + (pl->pos[1] + sp40[1]);
                sp50[2] = pl->pos[2] + sp40[2];
                func_551790(pl, 0, sp50, 1);
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 0);
            }
            if ((frame_check2(116.0f, pl, 0) != 0) && (frame_check2(192.0f, pl, 0) == 0)) {
                ex_atk_ck(pl, 1);
            }
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(116.0f, pl, 0) != 0) && (frame_check2(192.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 1);
        }
        break;
    }
}

void pl_at004(PLW *pl, u32 arg1) {
    f32 sp50[3];
    f32 sp40[3];
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x579, 4, 0);
            action_timer_calc(pl, 0);
            func_6362B0(pl, 3);
            pl->work615 = 1;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                switch (arg1) {
                case 1:
                    pl_chr_set2(pl, 0x582, 2, 0);
                    pl->flag12 = 1;
                    pl->work615 = 0;
                    break;
                case 2:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x58C, 4, 0);
                    break;
                case 3:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x58A, 4, 0);
                    break;
                case 4:
                    pl->work615 = 1;
                    pl_chr_set2(pl, 0x57D, 4, 0);
                    break;
                case 5:
                    pl->work615 = 1;
                    pl->ang[1] = pl->ang[1] + 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x590, 0, 0);
                    break;
                case 6:
                    pl->work615 = 1;
                    pl->ang[1] = pl->ang[1] - 0x2000;
                    pl->ang_y = pl->ang[1];
                    pl_chr_set2(pl, 0x592, 0, 0);
                    break;
                }
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    switch (arg1) {
                    case 1:
                        pl_chr_set2(pl, 0x579, 0, 0x5C);
                        break;
                    case 2:
                        pl_chr_set2(pl, 0x579, 2, 0x32);
                        break;
                    case 3:
                        pl_chr_set2(pl, 0x579, 2, 0x44);
                        break;
                    case 4:
                        pl_chr_set2(pl, 0x579, 2, 0x4A);
                        break;
                    case 5:
                        pl_chr_set2(pl, 0x579, 0, 0x46);
                        break;
                    case 6:
                        pl_chr_set2(pl, 0x579, 0, 0x34);
                        break;
                    }
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 3);
                    pl->work615 = 1;
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((pl->work019 != 0) && (pl->x07 == 0)) {
            pl->x07 = 1;
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                func_543690(pl, 0xF, 0);
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set(pl, 1, 6, 0);
                break;
            case 2:
            case 3:
                pl->x05++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 1);
                break;
            }
        } else {
            if ((pl->x07 == 0) && (frame_check(102.0f, pl, 0) != 0)) {
                sp50[0] = 0.0f;
                sp50[1] = 0.0f;
                sp50[2] = 150.0f;
                flvecApplyMat33(sp40, sp50, (f32 *)((u8 *)pl + 0x20));
                sp50[0] = pl->pos[0] + sp40[0];
                sp50[1] = 15.0f + (pl->pos[1] + sp40[1]);
                sp50[2] = pl->pos[2] + sp40[2];
                func_551790(pl, 0, sp50, 1);
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 0);
            }
            if ((frame_check2(108.0f, pl, 0) != 0) && (frame_check2(162.0f, pl, 0) == 0)) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    case 2:
        if (pl->x610 <= 0) {
            pl->x05 = s + 1;
            func_543690(pl, 0x1C, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((frame_check2(108.0f, pl, 0) != 0) && (frame_check2(162.0f, pl, 0) == 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at006(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl_chr_set2(pl, 0x57C, 0, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(18.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 7, 0xC);
            }
        } else if ((frame_check3(2.0f, 30.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
            pl->x07 = 0;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            switch (pl->x07) {
            case 0:
                Pl_act_set(pl, 1, 1, 0);
                pl_chr_set2(pl, 0x579, 0, 0x10);
                pl->work615 = 1;
                break;
            case 2:
                Pl_act_set(pl, 1, 1, 0);
                pl->work615 = 1;
                break;
            }
        }
        break;
    }
}
