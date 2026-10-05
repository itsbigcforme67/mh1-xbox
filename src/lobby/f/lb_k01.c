/* lb_k01 - lobby chat handlers 0x005D4AA0-0x005D4DF8: lb_pl_chat00, lb_pl_chat01, lb_pl_chat02, lb_pl_chat03. Whole file in lb_k.c. */
#include "lobby_f.h"
int frame_check2(f32, PLW *, int);
void Pl_basic_flagset();
void Lb_Pl_act_set2();















void lb_pl_chat00(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0xD2, 4, 0);
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (Pl_master_ck(pl) == 1) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_Pl_act_set2(pl, 1, 1, 0);
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}

void lb_pl_chat01(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0xCF, 0, 0);
        pl->work39C = 0;
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        return;
    case 1:
        if (F(s32, pl, 0x194) == 0) {
            Lb_pl_to_normal(pl, 0, 4, 0);
            pl->work8F0 = 1;
        }
    }
}

void lb_pl_chat02(PLW *pl) {
    int t;
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x26C, 6, 0);
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

void lb_pl_chat03(PLW *pl) {
    u8 s;
    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        Lb_pl_chr_set(pl, 0x26F, 6, 0);
        pl->work39C = 0;
        Lb_Pl_basic_flagset(pl, 0, 0, 0);
        return;
    case 1:
        if (Pl_master_ck(pl) == 1 && frame_check2(90.0f, pl, 0) != 0) {
            if (pl->sw.pow[0] >= 0x28) {
                Lb_Pl_act_set2(pl, 1, 4, 0);
                return;
            }
            if (PLU8(pl, 0x8C4) != 0) {
                Lb_Pl_chat_act_set(pl);
            }
        }
    }
}
