/* lb_j02 - lobby move handlers 0x005D09E0-0x005D0E60: lb_pl_mv045, lb_pl_mv046, lb_pl_mv047, lb_pl_mv048, lb_pl_mv051. Whole file in lb_j.c. */
#include "lobby_f.h"







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
