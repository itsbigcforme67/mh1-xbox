/* Player code (SLPM_654.95 0x0014FC40-0x00150490): status/attack/defense adjust, skills, resistances */
#include "pl.h"
#include "game.h"
#include "plf.h"
extern u8 Equip_Bonus[0x630];
s16 Get_equip_value(u8 kind);
s16 Pl_item_num_ck(PLW *, int);

s32 Pl_bari_ck(PLW *pl) {
    if (act_ck(pl, 0, 0x36) != 0 || act_ck(pl, 0, 0x48) != 0) {
        return 1;
    }
    return 0;
}

void Pl_status_set(PLW *pl) {
    u8 f = 0;
    int t;
    if (Pl_master_ck(pl) != 0) {
        if (pl->x7BA != 0) {
            f |= 1;
        }
        if (act_ck(pl, 2, 0x15) != 0) {
            f |= 2;
        }
        if (act_ck(pl, 2, 0x16) != 0) {
            f |= 4;
        }
        if (act_ck(pl, 2, 0x13) != 0) {
            f |= 8;
        }
        t = (s16)(pl->work6A4 + pl->work6A5);
        if (t > 0) {
            f |= 0x10;
        } else if (t < 0) {
            f |= 0x40;
        }
        t = (s16)(pl->work6A8 + pl->work6A9);
        if (t > 0) {
            f |= 0x20;
        } else if (t < 0) {
            f |= 0x80;
        }
        pl->work4D5 = f;
    }
}

void Pl_atck_adj_calc(PLW *pl) {
    int v = 0;
    if (Pl_master_ck(pl) == 1) {
        v += Get_equip_value(0);
        if (Pl_item_num_ck(pl, 0xA6) != 0) {
            v += 5;
        }
        v += pl->work6A4;
        v += pl->work6A5;
        if (Pl_Skill_ck(pl, 0x20) == 1) {
            v += 3;
        } else if (Pl_Skill_ck(pl, 0x21) == 1) {
            v += 5;
        }
        if (v < 0) {
            v = 1;
        }
        pl->work6AC = v;
        if (pl->work6AC >= 0x190) {
            pl->work6AC = 0x190;
        }
    }
    pl->atk_rate = (u32)pl->work6AC / 100.0f;
}

void Pl_def_adj_calc(PLW *pl) {
    int v = 1;
    v += Get_equip_value(1);
    if (Pl_item_num_ck(pl, 0xA7) != 0) {
        v += 5;
    }
    v += pl->work6A8;
    v += pl->work6A9;
    if (Pl_Skill_ck(pl, 0x25) == 1) {
        v += 5;
    } else if (Pl_Skill_ck(pl, 0x26) == 1) {
        v += 10;
    }
    if (v < 0) {
        v = 1;
    }
    if ((f32)pl->vital / (f32)pl->work792 <= 0.4f) {
        v += 0x1E;
    }
    pl->work6AE = v & 0xFF;
    pl->work7DC = (u32)pl->work6AE;
}

void Skill_set_PL(PLW *pl) {
    u8 *e = Equip_Bonus;
    s16 i;
    if (*e != 0xFF) {
        do {
            if ((e[1] == 0xFF || e[1] == pl->work352[0]) && (e[2] == 0xFF || e[2] == pl->work352[2])
                && (e[3] == 0xFF || e[3] == pl->work352[3]) && (e[4] == 0xFF || e[4] == pl->work352[4])
                && (e[5] == 0xFF || e[5] == pl->work352[5])) {
                pl->skill[0] = e[6];
                pl->skill[1] = e[7];
                pl->skill[2] = e[8];
                pl->skill[3] = e[9];
                pl->skill[4] = e[10];
                return;
            }
            e += 0xC;
        } while (*e != 0xFF);
    }
    pl->skill[0] = 0;
    pl->skill[1] = 0;
    pl->skill[2] = 0;
    pl->skill[3] = 0;
    pl->skill[4] = 0;
}

s32 Pl_Skill_ck(PLW *pl, int id) {
    s16 i;
    for (i = 0; i < 5; i++) {
        if (pl->skill[i] == (u8)id) {
            return 1;
        }
    }
    return 0;
}

void Pl_reg_calc(PLW *pl) {
    pl->work920 = (f32) Get_equip_value(2);
    pl->work924 = (f32) Get_equip_value(3);
    pl->work928 = (f32) Get_equip_value(4);
    pl->work92C = (f32) Get_equip_value(5);
    if (Pl_Skill_ck(pl, 1) == 1) {
        pl->work920 = 25.0f + pl->work920;
    }
    if (Pl_Skill_ck(pl, 2) == 1) {
        pl->work924 = 25.0f + pl->work924;
    }
    if (Pl_Skill_ck(pl, 3) == 1) {
        pl->work928 = 25.0f + pl->work928;
    }
    if (Pl_Skill_ck(pl, 4) == 1) {
        pl->work92C = 25.0f + pl->work92C;
    }
    if (Pl_Skill_ck(pl, 5) == 1) {
        pl->work920 = (f32) (pl->work920 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 6) == 1) {
        pl->work924 = (f32) (pl->work924 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 7) == 1) {
        pl->work928 = (f32) (pl->work928 - 10.0f);
    }
    if (Pl_Skill_ck(pl, 8) == 1) {
        pl->work92C = (f32) (pl->work92C - 10.0f);
    }
    if (pl->work920 < -100.0f) {
        pl->work920 = -100.0f;
    }
    if (pl->work924 < -100.0f) {
        pl->work924 = -100.0f;
    }
    if (pl->work928 < -100.0f) {
        pl->work928 = -100.0f;
    }
    if (pl->work92C < -100.0f) {
        pl->work92C = -100.0f;
    }
    if (!(pl->work920 <= 100.0f)) {
        pl->work920 = 100.0f;
    }
    if (!(pl->work924 <= 100.0f)) {
        pl->work924 = 100.0f;
    }
    if (!(pl->work928 <= 100.0f)) {
        pl->work928 = 100.0f;
    }
    if (!(pl->work92C <= 100.0f)) {
        pl->work92C = 100.0f;
    }
}
