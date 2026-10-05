/* work in progress; functions move to pl_nm.c once they compile */
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
