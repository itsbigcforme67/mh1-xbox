/* Player code (SLPM_654.95 0x0013DB40-0x0013E408): pl_mv043, 044, 046, 047, 049, 051, 052, 053 (action handlers: ladder/hole/gather states) */
#include "pl.h"
#include "game.h"
#include "plf.h"

void pl_mv043(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 2, 0, 0);
        pl->flag604 = 0;
        wall_vec_set(pl, 0);
        pl_chr_set2(pl, 0x22, 4, 0);
        break;
    case 1:
        if (frame_check2(96.0f, pl, 0) != 0) {
            Pl_act_set(pl, 0, 0x27, 0);
        }
        break;
    }
    kabe_hosei(pl, 1);
}

void pl_mv044(PLW *pl) {
    u8 s;

    pl->work8F0 = 1;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x1E, 4, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_mv046(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x195, -0xE, 0);
        break;
    case 1:
        if (frame_check2(130.0f, pl, 0) != 0) {
            pl->x05++;
            pl_chr_set2(pl, 0x19A, 4, 0);
        }
        break;
    case 2:
        if (frame_check2(60.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x2F, 0xC);
            } else {
                pl->x05++;
            }
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_mv047(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x19A, 0, 0x3C);
        if (Pl_master_ck(pl) == 1) {
            Ana_item_set(pl);
        }
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0x195, -4, 0xCE);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 0xA, 0);
        }
        break;
    }
}

void pl_mv049(PLW *pl, s32 arg1) {
    u8 s;
    u8 t;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x43E = 0;
        pl->work40C = 6;
        Pl_basic_flagset(pl, 0, 0, 0);
        Pl_stamina_calc(pl, -0x4B);
        pl->work39C = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x3F5, 2, 0);
            break;
        }
        pl_chr_set2(pl, 0x3F4, 2, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 8, 0);
            break;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->flag12 != 0)) {
            switch (pl->kind) {
            case 4:
                if ((pl->sw.an_trg & 0x20) && (frame_check3(10.0f, 42.0f, pl, 0) != 0) && (pl->x06 == 0)) {
                    pl->x06 = 1;
                    if (pl->sw.an_now & 0x800) {
                        pl->x06 = 2;
                    }
                    if (pl->sw.an_now & 0x400) {
                        pl->x06 = 3;
                    }
                }
                if (frame_check2(42.0f, pl, 0) != 0) {
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
            }
        }
        break;
    }
}

void pl_mv051(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl_chr_set2(pl, 0x1AA, -6, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            Pl_act_set2(pl, 0, 0x34, 0);
        }
        break;
    }
}

void pl_mv052(PLW *pl) {
    f32 sp20[3];
    u8 s;
    s32 w;
    s32 h;
    u16 r;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl->work08 = 0x78;
        pl_chr_set2(pl, 0x1AB, 0, 0);
        Eft06_set(4.0f, pl, 0, 0, 0xA);
        pl->x7BA = 0;
        pl->x7AA = 0;
        break;
    case 1:
        w = pl->work08 - 1;
        pl->work08 = w;
        if ((w <= 0) && (Pl_master_ck(pl) == 1)) {
            Pl_act_set2(pl, 0, 0x35, 0);
            break;
        }
        if ((pl->char0 == 0x1AB) && !((r = ran_suu(1)) & 0x7F)) {
            pl_chr_set2(pl, 0x1AC, 4, 0);
        } else if ((pl->char0 == 0x1AC) && (pl->work194 == 0)) {
            pl_chr_set2(pl, 0x1AB, 4, 0);
        }
        sp20[0] = 0.0f;
        sp20[1] = 20.0f;
        h = *(u16 *)&game_w.x1E % 60;
        sp20[2] = 0.0f;
        switch (h) {
        case 0:
        case 0xA:
        case 0x14:
            Eft06_set2(0.8f, pl, 4, 0x14, sp20);
            break;
        }
        Pl_vital_calc(pl, 1);
        break;
    }
}

void pl_mv053(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    pl->work4D4 = 0;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl->ang[1] = (pl->ang[1] + 0x8000) & 0xFFFF;
        pl->ang_y = pl->ang[1];
        pl_chr_set2(pl, 0x1AD, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}
