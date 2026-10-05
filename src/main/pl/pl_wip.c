/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"




















void pl_at028(PLW *pl) {
    u16 v;
    u32 c;
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x57B, 6, 0);
        pl->x06 = pl->work880 + 2;
        if (pl->x06 >= 5) {
            pl->x06 = 5;
        }
        pl->x07 = 0;
        pl->work08 = 0;
        pl->ang_y = pl->ang[1];
        func_6362B0(pl, 0xC);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        break;
    case 1:
        pl->x43E = 2;
        if (frame_check(44.0f, pl, 0) != 0) {
            pl->work08 = (u32)pl->work08 + 1;
            t = pl->x06 - 1;
            pl->x06 = t;
            if (t <= 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x57C, 0, 0);
                pl->cnt39A = pl->cnt39A + 3;
                func_6362B0(pl, 0xF);
                break;
            }
            c = pl->work08;
            switch (c) {
            case 0:
            case 1:
            case 2:
                if (pl->x07 != 0) {
                    if (pl->sw.an_now & 0x800) {
                        pl->ang[1] = pl->ang[1] + 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->ang[1] = pl->ang[1] - 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    Pl_act_set2(pl, 1, 0x3A, 4);
                }
                break;
            case 3:
            case 4:
            case 5:
                if (pl->x07 != 0) {
                    if (pl->sw.an_now & 0x800) {
                        pl->ang[1] = pl->ang[1] + 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->ang[1] = pl->ang[1] - 0x2000;
                        pl->ang_y = pl->ang[1];
                    }
                    Pl_act_set2(pl, 1, 0x4D, 4);
                }
                break;
            }
        } else {
            if (Pl_master_ck(pl) == 1) {
                v = pl->sw.an_now;
                if (v & 0x800) {
                    pl->ang_y = *(u16 *)&pl->ang_y + 0xC0;
                } else if (v & 0x400) {
                    pl->ang_y = *(u16 *)&pl->ang_y - 0xC0;
                }
                if (pl->sw.an_trg & 0x20) {
                    pl->x07++;
                }
            }
            if (frame_check(26.0f, pl, 0) != 0) {
                func_6362B0(pl, 0xD);
            }
            if (frame_check(52.0f, pl, 0) != 0) {
                func_6362B0(pl, 0xE);
            }
        }
        break;
    case 2:
        pl->x43E = 2;
        if (frame_check(38.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 1);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_at029(PLW *pl, s32 arg1) {
    f32 sp40[3];
    f32 sp30[3];
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        if (arg1 == 4) {
            pl_chr_set2(pl, 0x57D, 2, 0);
        } else {
            pl_chr_set2(pl, 0x57D, 4, 0);
        }
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 8);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (frame_check2(30.0f, pl, 0) == 0) {
            if (arg1 != 2) {
                if (arg1 == 3) {
                    goto go;
                }
            } else {
go:
                sp40[0] = 0.0f;
                sp40[1] = 0.0f;
                sp40[2] = 18.0f * flCos(2.0f * (3.1415927f * ((90.0f * (pl->work19C / 24.0f)) / 360.0f)));
                flvecApplyMat33(sp30, sp40, (f32 *)((u8 *)pl + 0x20));
                pl->pos[0] = pl->pos[0] + sp30[0];
                pl->pos[2] = pl->pos[2] + sp30[2];
            }
        }
        if ((arg1 == 0) || (arg1 == 3)) {
            if (frame_check(60.0f, pl, 0) != 0) {
                func_6362B0(pl, 9);
            }
            if (pl->work194 == 0) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (frame_check2(88.0f, pl, 0) != 0) {
                ex_atk_ck(pl, 0);
                break;
            }
        } else {
            if ((we02_hit_sub(pl) != 1) && (frame_check2(30.0f, pl, 0) != 0)) {
                pl->x05++;
                pl_chr_set2(pl, 0x57F, 0, 0);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if (frame_check3(32.0f, 46.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at030(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x57E, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0xA);
        pl->x43E = 0x3C;
        break;
    case 1:
        if (frame_check(74.0f, pl, 0) != 0) {
            func_6362B0(pl, 0xB);
        }
        if (frame_check(80.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 2);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(84.0f, 170.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at031(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x580, 0, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(32.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 0x51, 0xC);
            }
        } else if ((frame_check3(2.0f, 30.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_at032(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3F2, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->flag12 = 1;
        pl->work615 = 1;
        pl->x43E = 0x3C;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57D, 2, 0x2E);
            action_timer_calc(pl, 0);
            pl->work615 = 1;
        }
        break;
    case 2:
        if (frame_check(56.0f, pl, 0) != 0) {
            func_6362B0(pl, 0x22);
            pl->x05++;
        }
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check2(88.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}
