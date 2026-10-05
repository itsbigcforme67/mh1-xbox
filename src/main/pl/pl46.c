/* Player code (SLPM_654.95 0x0014B3F0-0x0014C1E0): pl_chat00..pl_chat16 (except dispatcher): chat/emote action states */
#include "pl.h"
#include "game.h"
#include "plf.h"
#include "flow.h"

void pl_chat00(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xD2, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (Pl_master_ck(pl) == 1) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 1, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat01(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0xCF, 0, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_chat02(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26C, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat03(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26F, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check2(90.0f, pl, 0) != 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat04(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x270, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xCF, 4, 0x3C);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_chat05(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x274, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if ((Pl_master_ck(pl) == 1) && (frame_check2(380.0f, pl, 0) != 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                Pl_act_set2(pl, 6, 6, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat06(PLW *pl) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x275, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl->x05 = s + 1;
            pl_chr_set2(pl, 0xD8, 4, 0x36);
        }
        break;
    case 2:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, 4, 0);
        }
        break;
    }
}

void pl_chat07(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x26E, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat08(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x273, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat09(PLW *pl, s32 arg1) {
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, chat09_chr_tbl_002F17C0[arg1].a, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        break;
    case 1:
        if (pl->work194 == 0) {
            pl_to_normal(pl, 0, chat09_chr_tbl_002F17C0[arg1].b, 0);
        }
        break;
    }
}

void pl_chat10(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x271, 4, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat11(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x276, 0x10, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat12(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x272, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}

void pl_chat16(PLW *pl) {
    s32 w;
    u8 s;

    s = pl->x05;
    switch (s) {
    case 0:
        pl->x05 = s + 1;
        pl_chr_set2(pl, 0x279, 6, 0);
        action_timer_calc(pl, 0);
        Pl_basic_flagset(pl, 0, 0, 0);
        pl->work08 = 0x3C;
        break;
    case 1:
        w = pl->work08;
        if (w > 0) {
            pl->work08 = w - 1;
        }
        if ((Pl_master_ck(pl) == 1) && (pl->work08 == 0)) {
            if ((pl->sw.pow[0] >= 0x28) || (Game_clear_ck(1) == 1)) {
                pl_to_normal(pl, 0, 4, 0);
                break;
            }
            if (pl->work8C4 != 0) {
                Pl_chat_act_set(pl);
            }
        }
        break;
    }
}
