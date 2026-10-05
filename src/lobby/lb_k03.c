/* lb_k03 - lobby chat handlers 0x005D5090-0x005D5750: lb_pl_chat07, lb_pl_chat08, lb_pl_chat09, lb_pl_chat10, lb_pl_chat11, lb_pl_chat12, lb_pl_chat16. Whole file in lb_k.c. */
#include "lobby.h"
int frame_check2(f32, PLW *, int);
void Pl_basic_flagset();
void Lb_Pl_act_set2();















void lb_pl_chat07(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x26E, 4, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat08(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x273, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat09(PLW *pl, int k) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, chat09_chr_tbl_0064E1C0[k * 2], 4, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, chat09_chr_tbl_0064E1C0[k * 2 + 1], 0);
            pl->work8F0 = 1;
        }
    }
}

void lb_pl_chat10(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x271, 4, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat11(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x276, 0x10, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat12(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x272, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat16(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x279, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        t = pl->work08;
        if (t > 0) {
            pl->work08 = t - 1;
        }
        if (Pl_master_ck(pl) == 1 && pl->work08 == 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_pl_to_normal(pl, 0, 4, 0);
                pl->work8F0 = 1;
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}
