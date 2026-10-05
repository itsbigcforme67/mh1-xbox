/* Player code (SLPM_654.95 0x00143790-0x00144538): pl_at014..pl_at026: weapon attack handlers (bow, lance, hammer, hunting horn?: charge/release: tame_pow_ck, tame_release, tame_com_ck) */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_at014(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 0, 0);
        func_6362B0(pl, 4);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_at020(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x32, 4, 0);
        func_6362B0(pl, 5);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_at021(PLW *pl) {
    u8 s;

    pl->x763 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 4, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work01C = pl->work01D;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

s32 we02_hit_sub(PLW *pl) {
    u8 s;
    u8 t;

    s = pl->x07;
    switch (s) {
    case 0:
        if (pl->work019 != 0) {
            Pl_set_quake_sub(pl, 0);
            switch (pl->work018) {
            case 0:
                pl->x07 = pl->x07 + 1;
                break;
            case 1:
                pl->x40A = 0;
                pl->work409 = 0;
                Pl_act_set2(pl, 1, 0x1F, 0);
                return 1;
            case 2:
            case 3:
                pl->x07++;
                pl_chr_set2(pl, pl->char0, 0, (s16)(pl->work19C - 2.0f));
                pl->work7ED = 2;
                Pl_set_quake_sub(pl, 1);
                break;
            }
        }
        break;
    case 1:
        if (pl->x610 <= 0) {
            pl->x07 = s + 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}

void pl_at022(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x579, 4, 0);
        func_6362B0(pl, 6);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (Pl_master_ck(pl) == 1) {
                if (frame_check3(50.0f, 80.0f, pl, 0) != 0) {
                    ex_atk_ck(pl, 1);
                }
                if ((frame_check2(72.0f, pl, 0) != 0) && (pl->sw.now & 0x80)) {
                    Pl_act_set(pl, 1, 0x4C, 0);
                }
            }
        }
        break;
    }
}

void pl_at023(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->work08 = 0;
        pl->x07 = 0;
        switch (arg1) {
        case 0:
            pl->x05++;
            pl_chr_set2(pl, 0x57A, 4, 0);
            action_timer_calc(pl, 0);
            Pl_basic_flagset(pl, 0, 1, 0);
            func_6362B0(pl, 7);
            pl->x43E = 0x1B;
            break;
        case 1:
            t = pl->x06;
            switch (t) {
            case 0:
                pl->x06 = t + 1;
                action_timer_calc(pl, 0);
                pl_chr_set2(pl, 0x581, 4, 0);
                Pl_basic_flagset(pl, 0, 1, 0);
                pl->flag12 = 1;
                break;
            case 1:
                if (pl->work194 == 0) {
                    pl->x05++;
                    pl_chr_set2(pl, 0x57A, 0, 0x18);
                    action_timer_calc(pl, 0);
                    func_6362B0(pl, 7);
                }
                break;
            }
            break;
        }
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (frame_check(66.0f, pl, 0) != 0) {
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 2);
            }
            if ((Pl_master_ck(pl) == 1) && (frame_check3(30.0f, 88.0f, pl, 0) != 0) && (pl->sw.an_trg & 0x3C) && (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC)) {
                pl->work08 = 1;
            }
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 6, 0);
                break;
            }
            if (pl->x610 == 0) {
                if (frame_check3(80.0f, 120.0f, pl, 0) != 0) {
                    ex_atk_ck(pl, 2);
                    break;
                }
                if ((frame_check2(70.0f, pl, 0) != 0) && (pl->work08 != 0)) {
                    Pl_act_set(pl, 1, 0x4E, 4);
                }
            }
        }
        break;
    }
}

s32 tame_pow_ck(PLW *pl) {
    return (pl->work87C / 30) & 0xFF;
}

void tame_release(PLW *pl) {
    s32 p;

    p = tame_pow_ck(pl) & 0xFF;
    switch (p) {
    case 0:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            Pl_act_set2(pl, 1, 0x39, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x21, 4);
        break;
    case 1:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            Pl_act_set2(pl, 1, 0x3A, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x1D, 4);
        break;
    default:
    case 2:
        if (pl_flag_ck(pl, 0x1000) != 0) {
            pl->work880 = tame_pow_ck(pl);
            Pl_act_set2(pl, 1, 0x1C, 4);
            break;
        }
        Pl_act_set2(pl, 1, 0x1E, 4);
        break;
    }
}

void tame_com_ck(PLW *pl) {
    if (Pl_master_ck(pl) != 0) {
        if ((pl->sw.trg & 0x40) && (pl->stamina >= 0x4B)) {
            Pl_act_set2(pl, 0, 0x1C, 4);
            return;
        }
        if ((pl->sw.now & 0x80) && (pl->stamina > 0)) {
            if (stick_pow_get(pl, 0) & 0xFF) {
                if (act_ck(pl, 1, 0x1A) == 0) {
                    pl->ang_y = stick_dir_set(pl, 0);
                    Pl_act_set(pl, 1, 0x1A, 0);
                } else {
                    pl->ang_y = stick_dir_set(pl, 0);
                }
            } else if ((act_ck(pl, 1, 0x18) == 0) && (act_ck(pl, 1, 0x1B) == 0) && (act_ck(pl, 1, 0x4C) == 0)) {
                Pl_act_set(pl, 1, 0x1B, 0);
            }
            if ((pl->sw.an_trg & 0x20) && (pl->work88C == 0)) {
                tame_release(pl);
            }
        } else {
            tame_release(pl);
        }
    }
}

void pl_at024(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->flag12 = 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->work08 = 0;
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x3EE, 4, 0);
            pl->work87C = 0;
            Eft06_set(4.0f, pl, 6, 0, 2);
            pl->x43E = 0x3C;
            break;
        case 1:
            pl->x05 = 3;
            pl_chr_set2(pl, 0x3EF, 8, 0);
            goto c3;
        case 2:
            pl->x05 = 1;
            pl_chr_set2(pl, 0x582, 4, 0);
            pl->work87C = 0;
            pl->x43E = 0x3C;
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x3EE, 0, 0xE);
            pl->work87C = 0;
            Eft06_set(4.0f, pl, 6, 0, 2);
            pl->x43E = 0x3C;
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x3EF, 4, 0);
            pl->x43E = 1;
        }
        break;
    case 3:
c3:
            tame_cnt_up(pl);
            if (Pl_master_ck(pl) == 1) {
                pl->work08 = pl->work08 + 1;
                if (pl->work08 >= 2) {
                    Pl_stamina_calc(pl, -1);
                }
            }
            tame_com_ck(pl);
        break;
    }
}

void pl_at025(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3F0, 0x14, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_at026(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x3ED, 8, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl_flag_set(pl, 0x02001000);
        pl->work08 = 0;
    case 1:
        tame_cnt_up(pl);
        if (Pl_master_ck(pl) == 1) {
            pl->work08 = pl->work08 + 1;
            if (pl->work08 >= 2) {
                Pl_stamina_calc(pl, -1);
            }
        }
        tame_com_ck(pl);
        break;
    }
}
