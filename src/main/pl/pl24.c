/* Player code (SLPM_654.95 0x00140100-0x001414B4): pl_mv083..pl_mv101: fishing/BBQ/flute/pitfall and other item-use action handlers */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_mv083(PLW *pl) {
    EMW *e;
    s16 i;
    u8 s;

    e = em_work;
    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    pl->work40C = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0;
        pl->work2DA = 0;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        i = 0;
        do {
            if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                pl->x06 = 1;
            }
            i++;
            e++;
        } while (i < 0x14);
        pl_chr_set2(pl, 0x327, 2, 0);
        break;
    case 1:
        if (pl->x06 == 0) {
            if (frame_check(118.0f, pl, 0) != 0) {
                pl->x05++;
                pl_chr_set2(pl, 0x328, 2, 0);
                break;
            }
            i = 0;
            do {
                if ((e->stg == pl->stg) && (e->x8B9 != 0) && (e->x87C == pl->id)) {
                    pl->x06 = 1;
                }
                i++;
                e++;
            } while (i < 0x14);
            break;
        }
        if (frame_check(pl->work1A8, pl, 0) != 0) {
            pl->x07++;
            if (pl->x07 >= 2) {
                pl->x05 = 3;
                pl_chr_set2(pl, 0x32A, 4, 0);
                if (Game_clear_ck(1) == 0) {
                    Pl_item_stack(pl, pl->work88A, -1);
                }
            }
        }
        break;
    case 2:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            set01_set(1, 0xB, (s16)pl->fish_time);
            adx_se_set(pl, 5);
            Pl_item_stack(pl, pl->work88A, -1);
            if ((s16)ItemStockRequest(pl, pl->fish_time, pl->x8E6, 0) == 3) {
                set01_set(1, 3, (s16)pl->fish_time);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    case 3:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 2, 0x24);
        }
        break;
    case 4:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv084(PLW *pl, s32 arg1) {
    u8 s;

    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x80000);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x329, 2, 0);
        break;
    case 1:
        if ((frame_check(120.0f, pl, 0) != 0) && (Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            if (arg1 == 0) {
                adx_se_set(pl, 8);
                Pl_item_stack(pl, pl->work88A, -1);
                set01_set(0, 3, 0);
            } else {
                set01_set(0, 0xD, 0);
            }
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

void pl_mv085(PLW *pl) {
    u8 s;
    u16 r;
    s16 t;

    ItemPickingDeclaration(pl, &pl->x8E6);
    pl->work40E = 2;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        r = ran_suu(1);
        pl_chr_set2(pl, 0x323, 4, r & 0x3F);
        pl->work8CA = 0;
        pl->work8D0 = 0;
        adx_se_set(pl, 0xA);
        break;
    case 1:
        pl->work8CA = pl->work8CA + 1;
        t = pl->work8CA;
        if (t >= 0x118) {
            pl->work8D0 = 3;
        } else if (t >= 0x10E) {
            pl->work8D0 = 2;
        } else if (t >= 0xB4) {
            pl->work8D0 = 1;
        } else {
            pl->work8D0 = 0;
        }
        if (Pl_master_ck(pl) == 1) {
            if (!(pl->sw.trg & 0x20)) {
                if (pl->work8CA >= 0x12C) {
                    goto go;
                }
            } else {
go:
                adx_se_stop(pl);
                switch (pl->work8D0) {
                case 0:
                    if (Game_clear_ck(1) == 0) {
                        set01_set(0, 0xB, 0);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                case 1:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x13, pl->x8E6, 0);
                        set01_set(1, 9, 0x13);
                        adx_se_set(pl, 0xB);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                case 2:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x14, pl->x8E6, 0);
                        set01_set(1, 9, 0x14);
                        switch (FLD8(system_w, 0x1A)) {
                        case 0:
                            adx_se_set(pl, 0xC);
                            break;
                        default:
                        case 2:
                        case 3:
                            adx_se_set(pl, 0xE);
                            break;
                        }
                    }
                    Pl_act_set2(pl, 0, 0x4D, 0);
                    break;
                case 3:
                    if (Game_clear_ck(1) == 0) {
                        Pl_item_stack(pl, 0x12, -1);
                        ItemStockRequest(pl, 0x15, pl->x8E6, 0);
                        set01_set(1, 0xA, 0x15);
                        adx_se_set(pl, 0xD);
                    }
                    Pl_act_set2(pl, 0, 0x4B, 0);
                    break;
                }
            }
        }
        break;
    }
}

void pl_mv086(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x19F, 0, 0);
        Pile_on(pl);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv087(PLW *pl) {
    u8 s;

    pl->work40E = 2;
    pl->work40C = 5;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work8C2 = 1;
        pl->work8F3 = 0;
        if (Pl_master_ck(pl) == 1) {
            adx_se_set(pl, 3);
        }
        break;
    case 1:
        if ((pl->work8C2 == 0) || (Game_clear_ck(1) == 1)) {
            pl->work40C = 0;
            pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void pl_mv088(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        switch (arg1) {
        case 0:
            pl_chr_set2(pl, 0x196, 4, 0);
            break;
        case 1:
            pl_chr_set2(pl, 0x1A0, 4, 0);
            break;
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x71, 0);
            } else {
                pl_to_normal(pl, 0, 4, 0);
            }
        }
        break;
    }
}

void pl_mv113(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        if (Pl_master_ck(pl) == 1) {
            Basic_item_set(pl);
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv089(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x199, 2, 0);
        pl->work39C = 0;
        break;
    case 1:
        if (frame_check(20.0f, pl, 0) != 0) {
            if (Pl_master_ck(pl) == 1) {
                Pl_act_set2(pl, 0, 0x14, 0xC);
            } else {
                pl->x05++;
                pl_chr_set2(pl, 8, 6, 0);
            }
        }
        break;
    case 2:
        break;
    }
}

void pl_mv091(PLW *pl) {
    u8 s;
    u8 v;
    u16 r;

    pl->work40C = 3;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->x8C6 = 0x3C;
        v = pl->work8C7;
        if (v < 7) {
            pl->work8C7 = v + 1;
            if ((pl->work8C7 >= 4) && ((r = ran_suu(1)) & 1)) {
                pl->work8C7 = 7;
            }
        } else {
            pl->work8C7 = 7;
        }
        pl->work08 = 0x3C;
        break;
    case 1:
        if (pl->x8C6 == 0) {
            pl_to_normal(pl, 0, 2, 0);
        }
        break;
    }
}

void pl_mv094(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->x06 = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl_flag_set(pl, 0x10000);
        pl->flag12 = 0;
        if (arg1 == 0) {
            pl_chr_set2(pl, 0x1A4, 4, 0);
        } else {
            pl_chr_set2(pl, 0x1A8, 4, 0);
        }
        pl->work39C = 0;
        break;
    case 1:
        switch (arg1) {
        case 0:
            if (pl->x06 >= 3 || ((frame_check2(44.0f, pl, 0) != 0) && (pl->sw.trg & 0x40))) {
                Pl_act_set2(pl, 0, 0x5F, 0);
            } else if (frame_check(pl->work1A8, pl, 0) != 0) {
                pl->x06++;
            }
            break;
        case 1:
            if (frame_check(pl->work1A8, pl, 0) != 0) {
                pl->x06++;
                if (pl->x06 >= 2) {
                    Pl_act_set2(pl, 0, 0x63, 0);
                }
            }
            break;
        }
        break;
    }
}

void pl_mv095(PLW *pl, s32 arg1) {
    u16 n;
    s32 h;
    u16 r;
    u8 s;

    s = pl->x05;
    n = 1;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        if (arg1 == 0) {
            n = 5;
            pl_chr_set2(pl, 0x1A5, 2, 0);
        } else {
            pl_chr_set2(pl, 0x1A9, 4, 0);
            Fue_item_set(pl);
            switch (pl->work88A) {
            case 0x8A:
                n = 0xC;
                break;
            case 0x8B:
                n = 0xC;
                break;
            case 0x8C:
                n = 8;
                break;
            case 0x8D:
                n = 8;
                break;
            }
        }
        if ((Pl_master_ck(pl) == 1) && (Game_clear_ck(1) == 0)) {
            h = (ran_suu(1) & 0xFFFF) % n;
            if ((h == 0) && ((Pl_Skill_ck(pl, 0x32) != 1) || ((r = ran_suu(1)) & 3)) && ((Pl_Skill_ck(pl, 0x33) != 1) || ((r = ran_suu(1)) & 1))) {
                set01_set(1, 0xE, (s16)pl->work88A);
                Pl_item_stack(pl, pl->work88A, -1);
            }
        }
        pl->work39C = 0;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv097(PLW *pl) {
    s32 n;
    s32 t;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->flag12 = 0;
        pl_chr_set2(pl, 0x19D, 4, 0);
        pl->work39C = 0;
        break;
    case 1:
        if ((frame_check(260.0f, pl, 0) != 0) && (Game_clear_ck(1) == 0)) {
            Pl_item_stack(pl, pl->work88A, -1);
            switch (pl->work88A) {
            case 0x69:
            case 0x5E:
                n = 0x96;
                break;
            case 0x9B:
                n = 0x64;
                break;
            case 0x6A:
                n = 0xC8;
                break;
            }
            if (Pl_Skill_ck(pl, 0x18) == 1) {
                t = (s16)n;
                n = (s16)(t + t / 4);
            }
            if (Pl_Skill_ck(pl, 0x19) == 1) {
                t = (s16)n;
                n = (s16)(t + t / 2);
            }
            Pl_slash_calc(pl, n);
        }
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_mv101(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Pl_basic_flagset(pl, 1, 0, 0);
        pl->pch_on = 1;
        if (arg1 == 0) {
            pl_chr_set2(pl, 7, 2, 0);
            break;
        }
        pl->x05++;
        pl_chr_set2(pl, 8, 2, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 8, 6, 0);
            sougun_adj_sub(pl, 8);
            break;
        }
        sougun_adj_sub(pl, 7);
        break;
    case 2:
        sougun_adj_sub(pl, 8);
        if (Pl_master_ck(pl) != 0) {
            if (!(pl->sw.trg & 0x40)) {
                if (Game_clear_ck(1) == 1) {
                    goto go;
                }
            } else {
go:
                Pl_view_reset(pl);
                Pl_act_set2(pl, 0, 0x1D, 0);
            }
        }
        break;
    }
}
