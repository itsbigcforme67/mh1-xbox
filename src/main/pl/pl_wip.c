/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv000(PLW *pl) {
    u16 c;

    c = pl->char0;
    switch (c) {
    case 0x1:
    case 0x3F:
        if (pl->work882 < 0x4C) {
            pl_chr_set2(pl, 0x193, 6, 0);
        } else if (pl->work39C >= 0xF0 && pl->work011 == 1 && c != 0x3F) {
            pl_chr_set2(pl, 0x3F, 2, 0);
        }
        break;
    case 0x15:
        if (pl->work194 == 0) {
            pl_chr_set2(pl, 5, 4, 0x90);
        }
        break;
    case 0x5:
        if (!(*(u16 *)&game_w.x1E & 0x1F)) {
            Eft02_set6(0.1f, pl, 8, 0x14);
        }
        break;
    case 0x193:
        if (pl->work882 > 0x4B) {
            pl_chr_set2(pl, 1, 4, 0);
        }
        break;
    case 0x3E9:
        if (pl->flag12 != 0) {
            if (pl->kind == 1 || pl->kind == 5) {
                blend_set(pl, 0x3ED, 0x3EE);
            }
        }
        break;
    }
    gun_adj_sub(pl);
    basic_com_ck(pl);
}

void pl_mv013(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work603 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl->x06 = 0x10;
            normal_char_set(pl, pl->x06, 0);
            break;
        }
        if (frame_check2(4.0f, pl, 0) != 0) {
            pl->work601 = 1;
        }
        break;
    case 2:
        s = pl->x06;
        pl->x06 = s - 1;
        if (s == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

void pl_mv008(PLW *pl, int arg1) {
    u8 s;
    u8 t;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        switch (arg1) {
        case 0:
            pl->x05 = s + 1;
            pl->x06 = 0;
            pl->work39C = 0;
            pl_chr_set2(pl, 7, 2, 0);
            Pl_basic_flagset(pl, 0, 0, 0);
            pl->flag12 = 0;
            t = pl->work56B;
            if (t & 0xF) {
                pl->work56B = t & 0xF0;
                func_549200(pl, 4);
            }
            break;
        case 1:
            pl->x05 = 2;
            pl->x06 = 0;
            if (pl->char0 == 0x19C) {
                pl_chr_set2(pl, 8, 8, 0);
            } else {
                pl_chr_set2(pl, 8, 2, 0);
            }
            action_timer_calc(pl, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            pl->flag12 = 0;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 8, 6, 0);
            Pl_basic_flagset(pl, 0x8001, 0, 0);
            action_timer_calc(pl, 0);
        }
        break;
    case 2:
        sit_com_ck(pl);
        break;
    }
}

void pl_mv004(PLW *pl, u32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        switch (arg1) {
        case 0:
            pl->flag12 = mv004_chr_tbl0[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl0[pl->kind].chr, mv004_chr_tbl0[pl->kind].mot, 0);
            break;
        case 1:
        case 3:
        case 4:
            pl->flag12 = mv004_chr_tbl1[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl1[pl->kind].chr, mv004_chr_tbl1[pl->kind].mot, 0);
            break;
        case 2:
            pl_flag_set(pl, 0x1000);
            pl->flag12 = mv004_chr_tbl2[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl2[pl->kind].chr, mv004_chr_tbl2[pl->kind].mot, 0);
            break;
        case 5:
            pl->flag12 = mv004_chr_tbl3[pl->kind].flag12;
            pl_chr_set2(pl, mv004_chr_tbl3[pl->kind].chr, mv004_chr_tbl3[pl->kind].mot, 0);
            break;
        }
        pl->x8C8 = 0;
        t = pl->work56B;
        if (t & 0xF) {
            pl->work56B = t & 0xF0;
            func_549200(pl, 4);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            switch (arg1) {
            case 0:
                pl_to_normal(pl, 0, mv004_chr_tbl0[pl->kind].next, 0);
                break;
            case 1:
                pl_to_normal(pl, 0, mv004_chr_tbl1[pl->kind].next, 0);
                break;
            case 2:
                pl_to_normal(pl, 0, mv004_chr_tbl2[pl->kind].next, 0);
                break;
            case 3:
                Pl_act_set2(pl, 0, 8, 2);
                break;
            case 4:
                if (Pl_master_ck(pl) == 1) {
                    item_action_set(pl, 1);
                } else {
                    pl_to_normal(pl, 0, mv004_chr_tbl0[pl->kind].next, 0);
                }
                break;
            case 5:
                pl_to_normal(pl, 0, mv004_chr_tbl3[pl->kind].next, 0);
                break;
            }
        }
        break;
    }
}

void pl_mv001(PLW *pl, u32 arg1) {
    u8 p;
    u8 t;

    if (pl->x05 == 0) {
        pl->x05++;
        pl->x06 = 0;
        pl->x07 = 10;
        pl->work08 = 10;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x1000);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 3, 6, 0);
            pl_flag_set(pl, 0x02000000);
            break;
        case 1:
            pl_chr_set2(pl, 2, 4, 0);
            break;
        case 2:
            switch (pl->kind) {
            case 0:
            default:
                pl_chr_set2(pl, 0x3EC, 4, 0);
                break;
            case 3:
                pl_chr_set2(pl, 0x3EC, 4, 0);
                break;
            case 2:
                pl_chr_set2(pl, 0x3EC, 8, 0);
                break;
            case 1:
            case 4:
            case 5:
                pl_chr_set2(pl, 0x3EC, 6, 0);
                break;
            }
            break;
        case 3:
            pl_chr_set2(pl, 0xA, 6, 0);
            break;
        case 5:
            pl_chr_set2(pl, 0xA, 2, 0);
            pl->st = 1;
            break;
        case 6:
            pl->work760 = 0x1E;
            if (pl->char0 != 3) {
                pl_chr_set2(pl, 3, 0, 0x26);
            }
            pl_flag_set(pl, 0x02000000);
            break;
        case 7:
            pl_chr_set2(pl, 0x31, 6, 0);
            break;
        case 8:
            pl_flag_set(pl, 0x8000);
            pl_chr_set2(pl, 0x3F3, 4, 0);
            break;
        }
        pl->work39C = 0;
    }
    if (Pl_master_ck(pl) == 0) {
        if (Online_ck() != 1) {
            goto go;
        }
    } else {
go:
        p = stick_pow_get(pl, pl->flag12);
        if (pl->x07 != 0) {
            pl->x07--;
        }
        if (pl->work08 > 0) {
            pl->work08--;
        }
        t = p;
        if (t == 0 && pl->work08 == 0) {
            if (pl->char0 == 0x3EC) {
                pl_to_normal(pl, 0, mv001_tbl[pl->kind], 0);
            } else if (pl->char0 == 3 || pl->char0 == 4) {
                if (pl->x07 == 0) {
                    pl->work8F0 = 1;
                    Pl_act_set(pl, 0, 0x2C, 0);
                } else {
                    pl_to_normal(pl, 0, 4, 0);
                }
            } else if (pl->char0 == 0x3F3) {
                Pl_act_set2(pl, 2, 0xC, 0);
            } else {
                pl_to_normal(pl, 0, 4, 0);
            }
            return;
        }
        switch (pl->char0) {
        case 2:
            if (frame_check(108.0f, pl, 0) != 0) {
                pl->x06++;
                if (pl->x06 >= 5) {
                    pl->x06 = 0;
                    pl_chr_set2(pl, 0x1D, 2, 0);
                }
            }
            break;
        case 3:
            if (t < 3) {
                pl->work8F0 = 1;
                Pl_act_set(pl, 0, 0x2C, 0);
                return;
            }
            break;
        case 0x1D:
            if (pl->work194 == 0) {
                pl_chr_set2(pl, 2, 6, 0x24);
            }
            break;
        case 0x3F3:
            if (!(pl->sw.now & 0x80)) {
                Pl_act_set2(pl, 2, 0xC, 0);
            }
            break;
        }
        if (pl->x714 != 0 && (arg1 != 5 || arg1 != 8)) {
            Pl_act_set(pl, 0, 0x3B, 0);
        }
        if (t != 0) {
            pl->ang_y = stick_dir_set(pl, 0);
        }
        if (arg1 != 8) {
            basic_com_ck(pl);
        } else {
            guard_atk_ck(pl);
        }
    }
}
