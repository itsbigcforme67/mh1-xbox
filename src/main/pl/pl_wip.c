/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv014(PLW *pl, s32 arg1) {
    int a2;
    f32 f;
    u8 s;
    u8 t;
    u8 idx;
    int off;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x191, 2, 0);
        } else {
            pl_chr_set2(pl, 0x1A1, 2, 0);
        }
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (arg1 != 0) {
            f = 20.0f;
        } else {
            f = 24.0f;
        }
        if (frame_check(f, pl, 0) != 0) {
            a2 = 0;
            if (pl->work016 == 0) {
                t = pl->work017;
                if (t == 1) {
                    a2 = 2;
                }
                if (t == 2) {
                    a2 = 1;
                }
            }
            switch (pl->work88A) {
            case 0x1A:
                func_628FB0(pl, 0, a2);
                break;
            case 0xA5:
                func_628FB0(pl, 7, a2);
                break;
            case 0x1B:
                func_628FB0(pl, 1, a2);
                break;
            case 0x1C:
                func_628FB0(pl, 5, a2);
                break;
            case 0x80:
                func_628FB0(pl, 6, a2);
                break;
            case 0x68:
                func_628FB0(pl, 8, a2);
                break;
            case 0x21:
                func_628FB0(pl, 9, a2);
                break;
            case 0x9F:
                func_628FB0(pl, 0xA, a2);
                break;
            }
            if (Game_clear_ck(1) == 0) {
                Pl_item_stack(pl, pl->work88A, -1);
            }
        }
        if (pl->work194 == 0) {
            switch (arg1) {
            case 0:
                pl_to_normal(pl, 0, 0xA, 0);
                break;
            case 1:
                pl_to_normal(pl, 0, 2, 0);
                break;
            case 2:
                if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
                    idx = game_w.x2F;
                    pl->x738 = 1;
                    pl->work73C = stage_start_pos[idx][0];
                    pl->work740 = stage_start_pos[idx][1];
                    pl->work744 = stage_start_pos[idx][2];
                    pl->work570 = stage_start_ang[idx];
                    pl->x73A = idx;
                    Pl_ofs_set(pl, &pl->work73C, pl->work570);
                    net_send_pl(pl, 5, 0);
                } else {
                    pl_to_normal(pl, 0, 2, 0);
                }
                break;
            }
        }
        break;
    case 2:
        break;
    }
}
