/* Lobby: player move handlers lb_pl_mvNNN (SLPM_654.95 lobby overlay 0x5D0690-). Whole file; runs split into lb_jNN.c */
#include "lobby_f.h"

void lb_pl_mv044(PLW *pl) {
    u8 s;
    if (PLU8(pl, 0x8EC) != 0) {
        Lb_act_set(pl, 0, 0x55);
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x1E, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, 0xA, 0);
        }
    }
}

void lb_pl_mv045(PLW *pl) {
    u8 s;
    PLU8(pl, 0x90B) = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl_chr_set2(pl, 0x3C, 4, 0);
        pl->work90A = 0;
        pl->work8F0 = 0;
        return;
    case 1:
        if (Pl_master_ck(pl) == 1 && ((F(u16, pl, 0x368) & 0x40) || pl->work90A != 0 || lbCommer[pl->work909].mac[0] == 0)) {
            Lb_Pl_act_set2(pl, 0, 0x2F, 0);
        }
    }
}

void lb_pl_mv046(PLW *pl) {
    int t;
    u8 s;
    PLU8(pl, 0x90B) = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        if (pl->char0 != 1) {
            pl_chr_set2(pl, 1, 6, 0);
        }
        pl->work90A = 0;
        pl->work08 = 0;
        return;
    case 1:
        t = pl->work08 + 1;
        pl->work08 = t;
        if (t >= 0x259 || pl->work90A != 0) {
            if (pl->work90A == 1) {
                Lb_Pl_act_set2(pl, 0, 0x30, 0);
            } else {
                Lb_pl_to_normal(pl, 0, 2, 0);
            }
        }
    }
}

void lb_pl_mv047(PLW *pl) {
    u8 s;
    PLU8(pl, 0x90B) = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x3D, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            pl->work8F0 = 1;
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
    }
}

void lb_pl_mv048(PLW *pl) {
    u8 s;
    PLU8(pl, 0x90B) = 1;
    ItemPickingDeclaration(pl, &pl->x8E6);
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        action_timer_calc(pl, 0);
        pl_chr_set2(pl, 0x280, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
    }
}

void pl_sleeping();
void lb_pl_mv051(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        lb_sys.x68 = 0x13;
        Lbc_set_prim(0, 0, 0);
        pl->x05 = pl->x05 + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        Lb_pl_chr_set(pl, 0x1AA, 6, 0);
        *(s8 *)0x3F36AB = 0;
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_Pl_act_set2(pl, 0, 0x34, 0);
        }
    }
}

void lb_pl_mv063(PLW *pl) {
    u8 s;
    if (PLU8(pl, 0x8EC) != 0) {
        Lb_act_set(pl, 0, 0x1F);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        pl->ang[1] = *(u16 *)&pl->ang_y;
        Lb_pl_chr_set(pl, 0x2A, 0, 0);
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_flag_set(pl, 0x1000);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_Pl_act_set2(pl, 0, 0x1F, 0);
        }
        break;
    }
}

void lb_pl_mv064(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        if (pl->char0 == 0x2A6) {
            pl->x05 = 2;
            return;
        }
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x2A5, 0x14, 0x1E);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x2A6, 6, 0);
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_act_set(pl, 0, 0x41);
                return;
            }
            lb_basic_com_ck(pl);
        }
        break;
    }
}

void lb_pl_mv065(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x2A7, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void lb_pl_mv077(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x262, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void lb_pl_mv079(PLW *pl) {
    int t;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        if (pl->char0 != 0x1AB) {
            Lb_pl_chr_set(pl, 0x1AB, 0, 0);
        }
        pl->work08 = 0x168;
        return;
    case 1:
        pl_sleeping(pl);
        t = pl->work08 - 1;
        pl->work08 = t;
        if (t <= 0) {
            Lb_Pl_act_set2(pl, 0, 0x35, 0);
        }
        break;
    }
}

void lb_pl_mv083(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0xCF, 2, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void lb_pl_mv084(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x26A, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x2B);
        }
        break;
    }
}

void lb_pl_mv043(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x282, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv085(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        if (pl->char0 != 1) {
            Lb_pl_chr_set(pl, 1, 9, 0);
        }
        break;
    case 1:
        if (F(s32, pl, 0x194) < 2) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x15, 4, 0);
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 5, 4, 0x90);
        }
        lb_basic_com_ck(pl);
        return;
    case 3:
        if (pl->work39C >= 0xF0) {
            Lb_act_set(pl, 0, 0x40);
            return;
        }
        lb_basic_com_ck(pl);
        break;
    }
}

void lb_pl_mv086(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25A, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv090(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x264, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv078(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x269, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv041(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x266, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv042(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x283, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv087(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25C, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x32A, 4, 0x2C);
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xD8, 6, 0x24);
        }
        break;
    case 3:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 8, 0);
            NPCZoomInCameraCancel();
            lb_sys.x6C = 0;
            lb_sys.x68 = 0;
        }
        break;
    }
}

void lb_pl_mv089(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_chidori_cnt_up(pl);
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x263, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_chr_set(pl, 0x261, 6, 0);
            Lb_act_set(pl, 0, 0x5D);
        }
        break;
    }
}

void lb_pl_mv092(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x268, 8, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->work08 = 0x3C;
            pl->x05 = pl->x05 + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1) {
            t = pl->work08;
            if (t <= 0) {
                if (F(u16, pl, 0x368) & 0x3C60) {
                    Lb_act_set(pl, 0, 0x4E);
                    return;
                }
            } else {
                pl->work08 = t - 1;
            }
        }
        break;
    }
}

void lb_pl_mv094(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x265, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x29);
        }
        break;
    }
}

void lb_pl_mv076(PLW *pl, int mode) {
    u16 b;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        pl->flag12 = 0;
        if (mode == 1) {
            pl->x05 = 2;
            pl->work08 = 0xE10;
            pl->work08 = 0x3C;
            if (pl->char0 != 0x261) {
                Lb_pl_chr_set(pl, 0x261, 6, 0);
            }
            return;
        }
        Lb_pl_chr_set(pl, 0x260, -4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            if (pl->id == game_w.master && game_w.stage != 0x4D && lb_sys.x66 != 0x10) {
                Lb_eat_to_bell(1, s);
                return;
            }
            pl->work08 = 0xE10;
            pl->work08 = 0x3C;
            pl->x05 = pl->x05 + 1;
            Lb_pl_chr_set(pl, 0x261, 4, 0);
        }
        break;
    case 2:
        if (F(u16, pl, 0x368) & 0x200) {
            mode = 0;
            if (lb_sys.x68 == 0) {
                Lb_Pl_act_set(pl, mode, 0x4D, 0);
                return;
            }
        }
        if (game_w.stage == 0x4D && Pl_master_ck(pl) == 1) {
            b = F(u16, pl, 0x368);
            if (b & 0x40) {
                Lb_act_set(pl, 0, 0x5A);
                return;
            }
            if (b & 0x20) {
                Lb_act_set(pl, 0, 0x59);
                return;
            }
            if (b & 0x2000) {
                Lb_act_set(pl, 0, 0x60);
                return;
            }
            if (b & 0x1000) {
                Lb_act_set(pl, 0, 0x5E);
                return;
            }
            if (b & 0x800) {
                Lb_act_set(pl, 0, 0x5C);
                return;
            }
            if (b & 0x400) {
                Lb_act_set(pl, 0, 0x54);
            }
        }
        break;
    }
}

void lb_pl_mv082(PLW *pl, s8 m) {
    int a;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        PLU8(pl, 0x4D4) = 0;
        pl->x05 = pl->x05 + 1;
        pl->work08 = 0;
        if (m == 0) {
            Lb_pl_chr_set(pl, 0x287, 0, 0);
            Lb_put_hint(0, 0x63);
            return;
        }
        Lb_pl_chr_set(pl, 0x288, 0, 0);
        return;
    case 1:
        if (m == 0) {
            pl->work08 = pl->work08 + 1;
            if (pl->work08 == 0xA0) {
                adx_se_set(pl, 7);
            }
        }
        if (F(s32, pl, 0x194) <= 0) {
            PLU8(pl, 0x4D4) = 1;
            pl_flag_clr(pl, 0x20000);
            a = pl->ang[1] + 0x7FFF + 1;
            pl->ang_y = a;
            pl->ang[1] = a & 0xFFFF;
            Lb_pl_to_normal(pl, 0, 0, 0);
            lb_sys.x68 = 0;
        }
        break;
    }
}

void lb_pl_mv095(PLW *pl, s8 m) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_chr_set(pl, 0x262, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            if (m == 0) {
                Lb_pl_chr_set(pl, 0x27D, 6, 0);
            } else {
                Lb_pl_chr_set(pl, 0x197, 6, 0x14);
                Eft06_set(4.0f, pl, 0, 8, 0xA);
            }
        }
        break;
    case 2:
        if (F(s32, pl, 0x194) <= 0) {
            u16 id;
            Lb_pl_to_normal(pl, 0, 6, 0);
            id = pl->id;
            if (id == game_w.master) {
                Lb_eat_to_end(id);
            }
        }
        break;
    }
}

void lb_pl_mv096(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 1, 0, 0);
        Lb_pl_chr_set(pl, 0x267, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
        }
        break;
    case 2:
        if (Pl_master_ck(pl) == 1 && (F(u16, pl, 0x368) & 0x3C60)) {
            Lb_act_set(pl, 0, 0x2A);
        }
        break;
    }
}

void lb_pl_mv097(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x27B, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x261, 6, 0);
        }
        break;
    case 2:
        if (frame_check2(10.0f, pl, 0) != 0) {
            switch (eatResult) {
            case 0:
                Lb_act_set(pl, 0, 0x62);
                return;
            case 1:
                Lb_act_set(pl, 0, 0x5F);
                return;
            case 2:
                Lb_act_set(pl, 0, 0x50);
                break;
            }
        }
        break;
    }
}

void lb_pl_mv098(PLW *pl) {
    u16 r;
    int t;
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x27C, 4, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0xD2, 0, 0x92);
            *(u16 *)&pl->ang_y = *(u16 *)&pl->ang_y + 0x4000;
            pl->ang[1] = pl->ang[1] + 0x4000;
            r = ran_suu(1);
            pl->work08 = (r & 0x1F) + 0x3C;
        }
        break;
    case 2:
        t = pl->work08 - 1;
        pl->work08 = t;
        if (t <= 0) {
            Lb_act_set(pl, 0, 0x53);
            Lb_eat_to_end();
        }
        break;
    }
}

void lb_pl_mv099(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x25D, 6, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) <= 0) {
            pl->x05 = s + 1;
            Lb_pl_chr_set(pl, 0x25E, 0, 0);
        }
        break;
    case 2:
        break;
    case 3:
        if (F(s32, pl, 0x194) <= 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void lb_pl_mv053(PLW *pl) {
    u8 s;
    if (pl->id != game_w.master) {
        Lb_act_set(pl, 0, 0);
        return;
    }
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work39C = 0;
        pl->ang[1] = (pl->ang[1] + 0x7FFF + 1) & 0xFFFF;
        pl->ang_y = pl->ang[1];
        Lb_pl_chr_set(pl, 0x1AD, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            lb_sys.x68 = 0;
            Lb_pl_to_normal(pl, 0, 6, 0);
        }
        break;
    }
}

void lb_pl_mv001(PLW *pl, int mode) {
    int t;
    u8 a;
    u8 s;
    u8 c;
    u16 ch;
    s = pl->x05;
    if (s == 0) {
        pl->x05 = s + 1;
        pl->x06 = 0;
        pl->x07 = 0xA;
        pl->work08 = 0xA;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_flag_set(pl, 0x1000);
        switch (mode) {
        case 0:
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x27) {
                    Lb_pl_chr_set(pl, 0x27, 6, 0);
                }
            } else if (pl->char0 != 3) {
                Lb_pl_chr_set(pl, 3, 6, 0);
            }
            Lb_pl_flag_set(pl, 0x02000000);
            break;
        case 1:
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x258) {
                    Lb_pl_chr_set(pl, 0x258, 4, 0);
                }
            } else if (pl->work011 == 1) {
                Lb_pl_chr_set(pl, 0x40, 4, 0);
            } else {
                Lb_pl_chr_set(pl, 2, 4, 0);
            }
            break;
        case 6:
            pl->work760 = 0x1E;
            if (PLU8(pl, 0x8EC) != 0) {
                if (pl->char0 != 0x27) {
                    Lb_pl_chr_set(pl, 0x27, 4, 0);
                }
            } else if (pl->char0 != 3) {
                Lb_pl_chr_set(pl, 3, 0, 0x3A);
            }
            Lb_pl_flag_set(pl, 0x02000000);
            break;
        case 9:
            Lb_pl_chr_set(pl, 0x258, 4, 0);
            break;
        }
        pl->work39C = 0;
    }
    a = Lb_stick_pow_get(pl);
    c = pl->x07;
    if (c != 0) {
        pl->x07 = c - 1;
    }
    t = pl->work08;
    if (t > 0) {
        pl->work08 = t - 1;
    }
    if (a == 0 && pl->work08 == 0) {
        ch = pl->char0;
        if (ch == 3 || ch == 4) {
            if (pl->x07 == 0) {
                Lb_Pl_act_set(pl, 0, 0x2C, 0);
            } else {
                Lb_pl_to_normal(pl, 0, 4, 0);
            }
        } else {
            Lb_pl_to_normal(pl, 0, 4, 0);
        }
        return;
    }
    if (pl->char0 == 3 && a < 5) {
        Lb_pl_to_normal(pl, 0, 4, 0);
        return;
    }
    if (pl->id == game_w.master && a != 0) {
        pl->ang_y = Lb_stick_dir_set(pl, 0);
    }
    lb_basic_com_ck(pl);
}

void lb_pl_mv031(PLW *pl) {
    LBV3 in;
    LBV3 out;
    u8 t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        Lb_pl_flag_set(pl, 0x600);
        pl->work39C = 0;
        pl->work760 = 0x1E;
        if (PLU8(pl, 0x8EC) != 0) {
            if (pl->char0 != 0x27) {
                pl_chr_set2(pl, 0x27, 4, 0);
            }
            pl->chr_spd0 = 2.0f;
            pl->chr_spd1 = 2.0f;
        } else if (pl->char0 == 3) {
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
        } else {
            pl_chr_set2(pl, 3, 0, 0x3A);
            pl->chr_spd0 = 2.5f;
            pl->chr_spd1 = 2.5f;
        }
        pl->ang_y = pl->ang[1];
        return;
    case 1:
        if (pl->id == game_w.master && pl->sw.pow[0] >= 0x28) {
            pl->ang_y = Lb_stick_dir_set(pl, 0);
        }
        in.x = 0;
        in.y = 0;
        if (PLU8(pl, 0x8EC) != 0) {
            *(s32 *)&in.z = 0x3F800000;
        } else {
            *(s32 *)&in.z = 0x40A00000;
        }
        flvecApplyMat33(&out, &in, (u8 *)pl + 0x20);
        pl->pos[0] = pl->pos[0] + out.x;
        pl->pos[2] = pl->pos[2] + out.z;
        t = Lb_stick_pow_get(pl);
        if (t != 5 && (game_w.master == pl->id || Online_ck() == 0)) {
            if (t != 0) {
                if (pl->work760 == 0) {
                    Lb_Pl_act_set(pl, 0, 2, 0);
                    return;
                }
                lb_basic_com_ck(pl);
                return;
            }
            Lb_Pl_act_set(pl, 0, 0x2C, 0);
            return;
        }
        lb_basic_com_ck(pl);
    }
}
