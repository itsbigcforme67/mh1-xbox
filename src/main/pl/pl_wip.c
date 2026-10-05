/* work in progress; functions move to pl_nm.c once they compile */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"




































void pl_at048(PLW *pl, s32 arg1) {
    f32 a;
    f32 b;
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        Pl_basic_flagset(pl, 0, 1, 0);
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x579, 4, 0);
            func_6362B0(pl, 0x15);
            break;
        case 1:
            pl_chr_set2(pl, 0x57A, 2, 0);
            func_6362B0(pl, 0x19);
            break;
        case 2:
            pl_chr_set2(pl, 0x57B, 2, 0);
            func_6362B0(pl, 0x1A);
            break;
        case 3:
            pl_chr_set2(pl, 0x579, 0xC, 0);
            func_6362B0(pl, 0x15);
            break;
        case 4:
            pl_chr_set2(pl, 0x579, 2, 0xA);
            func_6362B0(pl, 0x15);
            func_543690(pl, 8, 1);
            break;
        }
        break;
    case 1:
        if (pl->x06 == 0) {
            a = 10.0f;
            b = 50.0f;
            if (arg1 == 2) {
                a = 30.0f;
                b = 70.0f;
            }
            if ((frame_check2(a, pl, 0) != 0) && (frame_check2(b, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
                t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
                if (t < 0x4001) {
                    pl->x06 = 2;
                } else if (t < 0x9556) {
                    pl->x06 = 1;
                } else if (t < 0xD556) {
                    pl->x06 = 2;
                }
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (we04_hit_sub(pl) != 1) {
            switch (arg1) {
            case 0:
            case 3:
            case 4:
                if (frame_check2(24.0f, pl, 0) != 0) {
                    if (pl->x06 != 0) {
                        if (pl->x06 == 1) {
                            Pl_act_set(pl, 1, 0x37, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x33, 4);
                        }
                    } else if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            case 1:
                if (frame_check2(20.0f, pl, 0) != 0) {
                    if (pl->x06 != 0) {
                        if (pl->x06 == 1) {
                            Pl_act_set(pl, 1, 0x38, 4);
                        } else {
                            Pl_act_set(pl, 1, 0x33, 4);
                        }
                    } else if (frame_check2(50.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            case 2:
                if (frame_check2(64.0f, pl, 0) != 0) {
                    if (pl->x06 == 2) {
                        Pl_act_set(pl, 1, 0x33, 4);
                    } else if (frame_check2(92.0f, pl, 0) == 0) {
                        ex_atk_ck(pl, 0);
                    }
                }
                break;
            }
            if ((frame_check(52.0f, pl, 0) != 0) && (arg1 == 2)) {
                func_6362B0(pl, 0x1B);
            }
        }
        break;
    }
}

void pl_at049(PLW *pl, s32 arg1) {
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        pl->x07 = 0;
        switch (arg1) {
        case 0:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 2, 0);
            func_6362B0(pl, 0x16);
            break;
        case 1:
            pl->x05++;
            pl->flag12 = 1;
            pl_chr_set2(pl, 0x3F2, 2, 0);
            break;
        case 2:
            pl->x05 = 2;
            pl_chr_set2(pl, 0x57C, 4, 0);
            func_6362B0(pl, 0x16);
            break;
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x57C, 0, 8);
            func_6362B0(pl, 0x16);
        }
        break;
    case 2:
        if ((pl->x06 == 0) && (frame_check2(20.0f, pl, 0) != 0) && (frame_check2(42.0f, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
            t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if ((t > 0x4000) && (t < 0x9556)) {
                pl->x06 = 1;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(34.0f, pl, 0) != 0)) {
            if (pl->x06 != 0) {
                Pl_act_set(pl, 1, 0x46, 4);
                break;
            }
            if (frame_check2(42.0f, pl, 0) == 0) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    }
}

void pl_at050(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x580, 4, 0);
        Pl_set_quake_sub(pl, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(32.0f, pl, 0) != 0) {
                Pl_act_set2(pl, 1, 0x50, 0xC);
            }
        } else if ((frame_check3(14.0f, 40.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_at051(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x57D, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x17);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(20.0f, pl, 0) != 0) && (frame_check2(40.0f, pl, 0) != 0)) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at052(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x588, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x18);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 2, 0xC, 0);
            break;
        }
        we04_hit_sub(pl);
        break;
    }
}

void pl_at061(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x585, 4, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        func_6362B0(pl, 0x1D);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_at065(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x589, 0, 0);
        Pl_set_quake_sub(pl, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        break;
    case 1:
        if (pl->x06 != 0) {
            if (frame_check2(42.0f, pl, 0) != 0) {
                Pl_act_set(pl, 1, 0x52, 0xC);
            }
        } else if ((frame_check3(10.0f, 40.0f, pl, 0) != 0) && (pl->x06 == 0) && (pl->work88C == 0) && (pl->sw.an_trg & 0x3C)) {
            pl->x06 = 1;
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_at070(PLW *pl, s32 arg1) {
    s32 t;
    u8 s;
    u8 u;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x06 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x57E, 2, 0);
        } else {
            pl_chr_set2(pl, 0x57E, 2, 8);
        }
        func_6362B0(pl, 0x1E);
        break;
    case 1:
        if ((pl->x06 == 0) && (frame_check2(26.0f, pl, 0) != 0) && (frame_check2(38.0f, pl, 0) == 0) && (pl->sw.an_trg & 0x3C)) {
            t = (pl->sw.ang[1] + 0x2AAB) & 0xFFFF;
            if (t < 0x4001) {
                pl->x06 = 2;
            } else if (t < 0x9556) {
                pl->x06 = 1;
            } else if (t < 0xD556) {
                pl->x06 = 2;
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if ((we04_hit_sub(pl) != 1) && (frame_check2(26.0f, pl, 0) != 0)) {
            u = pl->x06;
            if (u == 1) {
                Pl_act_set(pl, 1, 0x48, 4);
                break;
            }
            if (u == 2) {
                Pl_act_set(pl, 1, 0x33, 4);
                break;
            }
            if (frame_check2(38.0f, pl, 0) == 0) {
                ex_atk_ck(pl, 0);
            }
        }
        break;
    }
}

void pl_at074(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x584, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x3C;
        func_6362B0(pl, 0x1F);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x43E = 0;
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(72.0f, 102.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at077(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x583, 0, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0x1C;
        func_6362B0(pl, 0x20);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check(40.0f, pl, 0) != 0) {
            vib_set_pl(pl, 2);
            Pl_set_quake_sub(pl, 1);
        }
        if (frame_check3(102.0f, 132.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 0);
        }
        break;
    }
}

void pl_at078(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x07 = 0;
        pl_chr_set2(pl, 0x584, 2, 0);
        Pl_basic_flagset(pl, 0, 1, 0);
        pl->x43E = 0;
        func_6362B0(pl, 0x21);
        pl->work08 = 0;
        break;
    case 1:
        if (we02_hit_sub(pl) != 1) {
            if (pl->work194 == 0) {
                if ((Pl_master_ck(pl) == 1) && (pl->work08 != 0) && (arg1 == 0)) {
                    Pl_act_set(pl, 1, 0x4F, 4);
                    break;
                }
                pl->x05++;
                pl_chr_set2(pl, 0x57A, 4, 0x4A);
                break;
            }
            if ((Pl_master_ck(pl) == 1) && (frame_check3(30.0f, 60.0f, pl, 0) != 0) && (pl->sw.an_trg & 0x3C) && (((pl->sw.ang[1] + 0x1555) & 0xFFFF) < 0xAAAC)) {
                pl->work08 = 1;
            }
            if (frame_check(52.0f, pl, 0) != 0) {
                vib_set_pl(pl, 2);
                Pl_set_quake_sub(pl, 2);
            }
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
            break;
        }
        if (frame_check3(80.0f, 120.0f, pl, 0) != 0) {
            ex_atk_ck(pl, 2);
        }
        break;
    }
}
